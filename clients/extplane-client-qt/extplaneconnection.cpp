#include <QDebug>
#include "extplaneconnection.h"
#include "clientdataref.h"
#include "simulateddatarefs/simulateddataref.h"
#include "extplaneclient.h"
#include "clientdataref.h"
#include "extplaneudpclient.h"
#include "../../util/console.h"
#include "../../protocoldefs.h"

ExtPlaneConnection::ExtPlaneConnection(QObject *parent) : BasicTcpClient(parent) {
    connect(this, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));
    connect(this, &BasicTcpClient::receivedLine, this, &ExtPlaneConnection::receivedLineSlot);
    connect(this, &BasicTcpClient::connectionChanged, this, &ExtPlaneConnection::connectionChangedSlot);
    setPort(EXTPLANE_PORT);
}

void ExtPlaneConnection::startConnection() {
    if(hostName().length()) {
        DEBUG << "Starting real connection to " << hostName() << "port" << port();
        emit connectionMessage(QString("Connecting to %1:%2..").arg(hostName()).arg(port()));
        BasicTcpClient::startConnection();
    } else {
        DEBUG << "Hostname not set yet - not connecting yet.";
        emit connectionMessage("No hostname set - not connecting yet.");
    }
}

void ExtPlaneConnection::stopConnection() {
    DEBUG << "Stopping connection";
    BasicTcpClient::disconnectFromHost();

    if(m_udpClient)
        m_udpClient->deleteLater();
    m_udpClient = nullptr;

    emit connectionMessage("Stopped connection");
}

void ExtPlaneConnection::connectionChangedSlot() { // Host or port changed -reconnect
    connected() ? stopConnection() : startConnection();
}

void ExtPlaneConnection::setUpdateInterval(double newInterval) {
    m_updateInterval = newInterval;
    if(server_ok & (m_updateInterval > 0)) {
        QString line = "extplane-set update_interval " + QString::number(newInterval);
        writeLine(line);
    }
}

void ExtPlaneConnection::socketError(QAbstractSocket::SocketError err) {
    Q_UNUSED(err)
    emit connectionMessage(QString("Socket error: %1").arg(errorString()));
    INFO << "Socket error:" << errorString();
    server_ok = false;
    emit connectedChanged(false);
    stopConnection();
}

void ExtPlaneConnection::registerClient(ExtPlaneClient* client) {
    clients.insert(client);
}

bool ExtPlaneConnection::isConnected() const
{
    return server_ok;
}

ClientDataRef *ExtPlaneConnection::subscribeDataRef(QString name, double accuracy) {
    ClientDataRef *ref = dataRefs[name];
    if(ref) {
        // DEBUG << QString("Ref %1 already subscribed %2 times").arg(name).arg(ref->subscribers());
        ref->setSubscribers(ref->subscribers()+1);
        if(accuracy < ref->accuracy()) {
            if(server_ok)
                subRef(ref); // Re-subscribe with higher accuracy
        }
    } else {
        // DEBUG << "Subscribing to new ref" << name << "accuracy" << accuracy;
        ref = createDataRef(name, accuracy);

        dataRefs[ref->name()] = ref;
        ref->setSubscribers(1);
        connect(ref, &ClientDataRef::unsubscribed, this, &ExtPlaneConnection::unsubscribeDataRef);
        connect(ref, &ClientDataRef::valueSet, this, &ExtPlaneConnection::setValueFromRef);
        if(server_ok)
            subRef(ref);
    }

    return ref;
}


ClientDataRef *ExtPlaneConnection::createDataRef(QString name, double accuracy) {
    return new ClientDataRef(this, name, accuracy);
}

void ExtPlaneConnection::unsubscribeDataRef(ClientDataRef *ref) {
    if(m_udpClient)
        m_udpClient->unsubscribedRef(ref);

    ref->setSubscribers(ref->subscribers() - 1);
    if(ref->subscribers() > 0) return;

    dataRefs.erase(ref->name());
    disconnect(ref, nullptr, this, nullptr);
    if(server_ok)
        writeLine("unsub " + ref->name());
    ref->setClient(nullptr);
    ref->deleteLater();
}

void ExtPlaneConnection::receivedLineSlot(QString & line) {
    if(!server_ok) { // Waiting for handshake..
        if(line=="EXTPLANE 1") {
            server_ok = true;
            emit connectionMessage(QString("Connected to ExtPlane at %1:%2").arg(hostName()).arg(port()));
            // DEBUG << "Setting update interval to " << m_updateInterval;
            setUpdateInterval(m_updateInterval);
            // Sub all refs
            for(auto &refPair : dataRefs) {
                // DEBUG << "Subscribing to ref" << refPair.second->name();
                subRef(refPair.second);
            }
            emit connectedChanged(true);
        }
        return;
    } else { // Handle updates
        if(line.startsWith("EXTPLANE-WARNING")) {
            emit extplaneWarning(line.mid(17));
        } else {
            QStringList cmd = line.split(" ", Qt::SkipEmptyParts);
            if(cmd.size()>=2) { // Normally 3, but can be 2 if a data dataref updates to be empty
                if(cmd.value(0)=="EXTPLANE-VERSION" && cmd.length() == 2) {
                    INFO << "Connected to ExtPlane version" << cmd.value(1);
                    m_extplaneVersion = cmd.value(1).toInt(); // Nothing done with this currently.
                    if(m_extplaneVersion != EXTPLANE_VERSION) {
                        INFO << "Warning: Server uses extplane version" << m_extplaneVersion << "and we are" << EXTPLANE_VERSION;
                    }
                } else if(cmd.value(0)=="CLIENT-ID" && cmd.length() == 2) {
                    INFO << "Client id set to" << cmd.value(1);
                    m_clientId = cmd.value(1).toUInt();
                } else if(cmd.value(0)=="udp") {
                    ClientDataRef *ref = dataRefs.at(cmd.value(3));
                    if(ref) {
                        quint16 id = cmd.value(1).toUInt();
                        ref->setUdpId(id);
                        if(!m_udpClient) {
                            if(!m_clientId) {
                                INFO << "Warning: No client id received! UDP receiving all updates";
                            }
                            m_udpClient = new ExtPlaneUdpClient(m_clientId, this);
                        }
                        m_udpClient->subscribedRef(ref);
                        // INFO << "Tracking ref via UDP" << cmd.value(3) << id;
                    } else {
                        INFO << "Can't find dataref" << cmd.value(3);
                    }
                } else {
                    ClientDataRef *ref = dataRefs[cmd.value(1)];
                    if(ref) {
                        if ((cmd.value(0)=="ufa" || cmd.value(0)=="uia") && cmd.size() == 3) {
                            // Array dataref
                            QString arrayString = cmd.value(2);
                            Q_ASSERT(arrayString[0]=='[' && arrayString[arrayString.length()-1]==']');
                            arrayString = arrayString.mid(1, arrayString.length()-2);
                            QStringList arrayValues = arrayString.split(',');
                            ref->updateValue(arrayValues);
                        } else if (((cmd.value(0)=="uf") || (cmd.value(0)=="ui") || (cmd.value(0)=="ud") ) && cmd.size() == 3) {
                            // Single value dataref
                            ref->updateValue(cmd.value(2));
                        } else if (cmd.value(0)=="ub") { // cmd size can be 2 or 3
                            // Data dataref
                            if(ref->dataFormat().contains("binary")) {
                                QByteArray rawArray = QByteArray::fromBase64(cmd.value(2).toUtf8());
                                QStringList values;
                                for(int i=0;i<rawArray.size();i++) {
                                    char value = rawArray[i];
                                    values.append(QChar(value));
                                }
                                ref->updateValue(values);
                            } else {
                                QString value;
                                if(ref->modifiers().contains("string")) {
                                    if(line.count("\"") >= 2) {
                                        value = line.mid(line.indexOf("\"") + 1);
                                        value = value.mid(0, value.lastIndexOf("\""));
                                    }
                                }
                                ref->updateValue(value);
                            }
                        } else {
                            INFO << "Unsupported ref type " << cmd.value(0);
                        }
                    } else {
                        INFO << "Ref not subscribed " << cmd.value(2);
                    }
                }
            }
        }
    }
}

void ExtPlaneConnection::subRef(ClientDataRef *ref) {
    Q_ASSERT(server_ok);
    QString subLine = "sub " + ref->name();
    if(ref->accuracy() != 0) {
        subLine += " " + QString::number(ref->accuracy());
    }
    writeLine(subLine);
}

void ExtPlaneConnection::writeLine(const QString &line) {
    if(!server_ok) {
        emit connectionMessage(QString("Connection not ok! Tried to write %1").arg(line));
        DEBUG << "Connection not ok! Tried to write" << line;
        return;
    }
    write(line.toUtf8());
    write("\n");
}

void ExtPlaneConnection::keyPress(int id) {
    QString line = "key " + QString::number(id);
    writeLine(line);
}

void ExtPlaneConnection::buttonPress(int id) {
    QString line = "but " + QString::number(id);
    writeLine(line);
}

void ExtPlaneConnection::buttonRelease(int id) {
    QString line = "rel " + QString::number(id);
    writeLine(line);
}

void ExtPlaneConnection::commandOnce(QString name) {
    QString line = "cmd once " + name;
    writeLine(line);
}

void ExtPlaneConnection::commandBegin(QString name) {
    QString line = "cmd begin " + name;
    writeLine(line);
}

void ExtPlaneConnection::commandEnd(QString name) {
    QString line = "cmd end " + name;
    writeLine(line);
}

void ExtPlaneConnection::setValue(QString name, QString value) {
    if(value.isEmpty()) {
        DEBUG << "Trying to set empty value for ref" << name;
        return;
    }

    QString line = QString("set %1 %2").arg(name, value);
    writeLine(line);
}

void ExtPlaneConnection::setValues(QString name, QStringList values) {
    QString line = "set " + name + " [" + values.join(",") + "]";
    writeLine(line);
}

void ExtPlaneConnection::setValueFromRef(ClientDataRef *ref) {
    if(ref->isArray()) {
        setValues(ref->name(), ref->values());
    } else {
        if(ref->modifiers().contains("string")) {
            setValue(ref->name(), "\"" + ref->value() + "\""); // Add quotes to strings
        } else {
            setValue(ref->name(), ref->value());
        }
    }
}
