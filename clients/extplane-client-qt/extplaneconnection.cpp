#include <QDebug>
#include "extplaneconnection.h"
#include "clientdataref.h"
#include "simulateddatarefs/simulateddataref.h"
#include "extplaneclient.h"
#include "../../util/console.h"

ExtPlaneConnection::ExtPlaneConnection(QObject *parent) : BasicTcpClient(parent)
  , server_ok(false)
  , updateInterval(0.333)
{
    connect(this, SIGNAL(connectedChanged(bool)), this, SLOT(connectedChanged(bool)));
    connect(this, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));
    connect(this, &BasicTcpClient::receivedLine, this, &ExtPlaneConnection::receivedLineSlot);
    setHostName("localhost");
    setPort(51000);
}

void ExtPlaneConnection::startConnection() {
    DEBUG << "Starting real connection";
    emit connectionMessage("Starting real connection");
    BasicTcpClient::startConnection();
}

void ExtPlaneConnection::stopConnection() {
    DEBUG << "Stopping real connection";
    BasicTcpClient::disconnectFromHost();
    qDeleteAll(dataRefs.values());
    dataRefs.clear();
    emit connectionMessage("Stopped real");
}

void ExtPlaneConnection::setUpdateInterval(double newInterval) {
    updateInterval = newInterval;
    if(server_ok & (updateInterval > 0)) {
        QString line = "extplane-set update_interval " + QString::number(newInterval);
        writeLine(line);
    }
}

void ExtPlaneConnection::connectedChanged(bool connected) {
    if(connected) {
        emit connectionMessage("Connected to ExtPlane, waiting for handshake");
    } else {
        emit connectionMessage("Socket disconnected");
    }
}

void ExtPlaneConnection::socketError(QAbstractSocket::SocketError err) {
    Q_UNUSED(err);
    INFO << "Socket error:" << errorString();
    server_ok = false;
}

void ExtPlaneConnection::registerClient(ExtPlaneClient* client) {
    clients.append(client);
}

ClientDataRef *ExtPlaneConnection::subscribeDataRef(QString name, double accuracy) {
    ClientDataRef *ref = dataRefs.value(name);
    if(ref) {
        DEBUG << "Ref already subscribed";
        ref->setSubscribers(ref->subscribers()+1);
        if(accuracy < ref->accuracy()) {
            if(server_ok)
                subRef(ref); // Re-subscribe with higher accuracy
        }
    } else {
        ref = createDataRef(name, accuracy);

        dataRefs[ref->name()] = ref;
        ref->setSubscribers(1);
        connect(ref, SIGNAL(unsubscribed(ClientDataRef*)), this, SLOT(unsubscribeDataRef(ClientDataRef*)));
        connect(ref, SIGNAL(valueSet(ClientDataRef*)), this, SLOT(setValue(ClientDataRef*)));
        if(server_ok)
            subRef(ref);
    }

    // DEBUG << name << ref->subscribers() << server_ok;
    return ref;
}


ClientDataRef *ExtPlaneConnection::createDataRef(QString name, double accuracy) {
    return new ClientDataRef(this, name, accuracy);
}

void ExtPlaneConnection::unsubscribeDataRef(ClientDataRef *ref) {
    // DEBUG << ref << ref->name() << ref->subscribers();
    ref->setSubscribers(ref->subscribers() - 1);
    if(ref->subscribers() > 0) return;
    // DEBUG << "Ref not subscribed by anyone anymore";
    dataRefs.remove(ref->name());
    disconnect(ref, nullptr, this, nullptr);
    if(server_ok)
        writeLine("unsub " + ref->name());
    // DEBUG << "Deleting ref " << ref->name() << ref;
    ref->deleteLater();
}

void ExtPlaneConnection::receivedLineSlot(QString & line) {
    //DEBUG << line;
    if(!server_ok) { // Waiting for handshake..
        if(line=="EXTPLANE 1") {
            server_ok = true;
            emit connectionMessage(QString("Connected to ExtPlane at %1:%2").arg(hostName()).arg(port()));
            setUpdateInterval(updateInterval);
            // Sub all refs
            for(ClientDataRef *ref : dataRefs)
                subRef(ref);
        }
        return;
    } else { // Handle updates
        QStringList cmd = line.split(" ", QString::SkipEmptyParts);
        if(cmd.size()==3) {
            ClientDataRef *ref = dataRefs.value(cmd.value(1));
            if(ref) {
                if (cmd.value(0)=="ufa" || cmd.value(0)=="uia"){
                    // Array dataref
                    QString arrayString = cmd.value(2);
                    Q_ASSERT(arrayString[0]=='[' && arrayString[arrayString.length()-1]==']');
                    arrayString = arrayString.mid(1, arrayString.length()-2);
                    QStringList arrayValues = arrayString.split(',');
                    ref->updateValue(arrayValues);
                } else if ((cmd.value(0)=="uf")||(cmd.value(0)=="ui")||(cmd.value(0)=="ud")) {
                    // Single value dataref
                    ref->updateValue(cmd.value(2));
                } else if (cmd.value(0)=="ub") {
                    // Data dataref
                    ref->updateValue(QByteArray::fromBase64(cmd.value(2).toUtf8()));
                } else {
                    INFO << "Unsupported ref type " << cmd.value(0);
                }
            } else {
                INFO << "Ref not subscribed " << cmd.value(2);
            }
        }
    }
}

void ExtPlaneConnection::subRef(ClientDataRef *ref) {
    // DEBUG << ref->name();
    Q_ASSERT(server_ok);
    QString subLine = "sub " + ref->name();
    if(ref->accuracy() != 0) {
        subLine += " " + QString::number(ref->accuracy());
    }
    writeLine(subLine);
}

void ExtPlaneConnection::writeLine(QString line) {
    if(!server_ok) {
        DEBUG << "Connection not ok! Tried to write" << line;
        return;
    }
    line +="\n";
    write(line.toUtf8());
    // qDebug() << Q_FUNC_INFO << line.trimmed();
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
    QString line = "set " + name + " " + value;
    writeLine(line);
}

void ExtPlaneConnection::setValues(QString name, QStringList values) {
    QString line = "set " + name + " [" + values.join(",") + "]";
    writeLine(line);
}

void ExtPlaneConnection::setValue(ClientDataRef *ref) {
    if(!ref->isArray()) {
        setValue(ref->name(), ref->value());
    } else {
        setValues(ref->name(), ref->values());
    }
}
