#include "tcpclient.h"
#include "tcpserver.h"
#include "datarefs/dataref.h"
#include "datarefs/floatdataref.h"
#include "datarefs/floatarraydataref.h"
#include "datarefs/intdataref.h"
#include "datarefs/intarraydataref.h"
#include "datarefs/doubledataref.h"
#include "datarefs/datadataref.h"
#include "datarefprovider.h"
#include "console.h"
#include "udpsender.h"

TcpClient::TcpClient(TcpServer *parent,
                     QTcpSocket *socket,
                     DataRefProvider *refProvider, quint8 clientId) : QObject(parent)
  , m_socket(socket)
  , m_refProvider(refProvider)
  , m_tcpserver(parent)
  , m_clientId(clientId)
{
    INFO << "Client connected from " << socket->peerAddress().toString();
    connect(m_socket, SIGNAL(readyRead()), this, SLOT(readClient()));
    connect(m_socket, SIGNAL(disconnected()), this, SLOT(disconnectClient()));
    connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));

    QByteArray block;
    QTextStream out(&block, QIODevice::WriteOnly);
    out << "EXTPLANE " << EXTPLANE_PROTOCOL << "\n";
    out << "EXTPLANE-VERSION " << EXTPLANE_VERSION << "\n";
    out << "CLIENT-ID " << m_clientId << "\n";
    out.flush();
    m_socket->write(block);
}

TcpClient::~TcpClient() {
    DEBUG;
    // disconnectClient(); nope, in destructor this crashes
}

void TcpClient::socketError(QAbstractSocket::SocketError err) {
    Q_UNUSED(err)
    INFO << "Socket error:" << m_socket->errorString();

    // deleteLater(); // WTF? No?
}

void TcpClient::disconnectClient() {
    INFO << Q_FUNC_INFO << this << m_subscribedRefs.size();
    while(!m_subscribedRefs.empty()) {
        DataRef *ref = *m_subscribedRefs.begin();
        m_subscribedRefs.erase(ref);
        ref->disconnect(this);
        m_refProvider->unsubscribeRef(ref);
    }
    for(int but : m_heldButtons)
        m_refProvider->buttonRelease(but);
    m_socket->disconnectFromHost();
    emit discoed(this);
    deleteLater();
}

void TcpClient::extplaneWarning(QString message) {
    if(m_socket->isOpen()) {
        m_socket->write(QString("EXTPLANE-WARNING %1\n").arg(message).toUtf8());
    }
}

void TcpClient::flightLoop()
{
    if(m_udpSender)
        m_udpSender->flightLoop();
}

void TcpClient::sendUdpInfo(DataRef *ref)
{
    if(m_socket->isWritable()) {
        if(ref->udpId()) {
            m_socket->write(QString("udp %1 %2 %3\n").arg(ref->udpId()).arg(ref->typeString()).arg(ref->name()).toUtf8());
        } else {
            extplaneWarning(QString("Dataref %1 not available via UDP").arg(ref->name()));
        }
    }
}

void TcpClient::readClient() {
    while(m_socket->canReadLine()) {
        QByteArray lineBA = m_socket->readLine();
        QString line = QString(lineBA).trimmed();

        if(line.isEmpty()) break;

        // Split the command in strings
        QStringList subLine = line.split(" ", QString::SkipEmptyParts);
        QString command = subLine.value(0);
        if(command == "disconnect") {
            DEBUG << "killing this client connection";
            disconnectClient();
        } else if(command == "sub" || command == "get") { // Subscribe or get command
            if(subLine.length() >= 2) {
                QString refName = subLine[1].trimmed();

                double accuracy = 0;
                bool accuracy_ok = false;
                if(subLine.length() >= 3) {
                    accuracy = subLine[2].toDouble(&accuracy_ok);
                    if(!accuracy_ok) {
                        extplaneWarning(QString("WARNING: Unable to parse accuracy value: %1 - accuracy set to 0").arg(subLine[2]));
                    }
                }

                DataRef *ref = getSubscribedRef(refName);
                if(!ref) { // Ref not subscribed yet, try to subscribe
                    ref = m_refProvider->subscribeRef(refName);
                    if(ref) { // Succesfully subscribed
                        connect(ref, &DataRef::changed, this, &TcpClient::refChanged);
                        m_subscribedRefs.insert(ref);
                        ref->setAccuracy(accuracy);

                        if(ref->type() == extplaneRefTypeFloat) {
                            m_refValueF[ref] = qobject_cast<FloatDataRef*>(ref)->value();
                        } else if(ref->type() == extplaneRefTypeInt) {
                            m_refValueI[ref] = qobject_cast<IntDataRef*>(ref)->value();
                        } else if(ref->type() == extplaneRefTypeDouble) {
                            m_refValueD[ref] = qobject_cast<DoubleDataRef*>(ref)->value();
                        } else if(ref->type() == extplaneRefTypeFloatArray) {
                            m_refValueFA[ref] = qobject_cast<FloatArrayDataRef*>(ref)->value();
                        } else if(ref->type() == extplaneRefTypeIntArray) {
                            m_refValueIA[ref] = qobject_cast<IntArrayDataRef*>(ref)->value();
                        } else if(ref->type() == extplaneRefTypeData) {
                            m_refValueB[ref] = qobject_cast<DataDataRef*>(ref)->value();
                        }
                        INFO << "Subscribed to " << ref->name() << ", accuracy " << accuracy << ", type " << ref->typeString() << ", valid " << ref->isValid();
                    }
                } else { // Ref already subscribed - update accuracy
                    INFO << "Updating " << refName << " accuracy to " << accuracy;
                    ref->setAccuracy(accuracy);
                }
                // ref can be null now!
                if(ref && ref->isValid()) {
                    sendRef(ref); // Force update

                    // Send UDP info
                    if(ref->modifiers().contains("udp")) {
                        if(!m_udpSender) {
                            m_udpSender = new UdpSender(m_refProvider, m_socket->peerAddress(), m_clientId, this);
                        }
                        if(!ref->udpId()) {
                            ref->setUdpId(m_tcpserver->reserveUdpId());
                        }
                        m_udpSender->subscribedRef(ref);
                        sendUdpInfo(ref);
                    }
                }
                if(command == "get" && ref) ref->setUnsubscribeAfterChange();
            } else {
                extplaneWarning(QString("Invalid sub command"));
            }
        } else if(command == "unsub") {
            QString refName = subLine.value(1);
            unsubscribeRef(refName);
        } else if(command == "set") {
            if(subLine.size() >= 3) {
                QString refName = subLine.value(1);
                DataRef *ref = getSubscribedRef(refName);

                if (ref) {
                    if(ref->isWritable()) {
                        if(ref->type() == extplaneRefTypeData) {
                            // Data datarefs are more difficult to set.
                            if(ref->modifiers().contains("string")) {
                                if(line.count("\"") >= 2) {
                                    // Strip quotes
                                    QString value = line.mid(line.indexOf("\"") + 1);
                                    value = value.mid(0, value.lastIndexOf("\""));
                                    ref->setValue(value);
                                } else {
                                    INFO << "Warning: No quotes in string data dataref value";
                                }
                            } else {
                                INFO << "Warning: setting data datarefs other than strings not supported yet.";
                            }
                        } else {
                            QString refValue = subLine.value(2).trimmed();
                            ref->setValue(refValue);
                        }
                        m_refProvider->changeDataRef(ref);
                    } else {
                        extplaneWarning(QString("Ref %1 is not writable!").arg(ref->name()));
                    }
                }
            } else {
                extplaneWarning(QString("Invalid set command"));
            }
        } else if(command == "key") {
            if(subLine.size() == 2) {
                bool ok;
                int keyId = subLine.value(1).toInt(&ok);
                if(ok)
                    m_refProvider->keyStroke(keyId);
            } else {
                extplaneWarning(QString("Invalid key command"));
            }
        } else if(command == "but") {
            if(subLine.size() == 2) {
                bool ok;
                int keyId = subLine.value(1).toInt(&ok);
                if(ok) {
                    m_refProvider->buttonPress(keyId);
                    m_heldButtons.insert(keyId);
                }
            } else {
                extplaneWarning("Invalid but command");
            }
        } else if(command == "rel") {
            if(subLine.size() == 2) {
                bool ok;
                int keyId = subLine.value(1).toInt(&ok);
                if(ok && m_heldButtons.find(keyId) != m_heldButtons.end()) {
                    m_refProvider->buttonRelease(keyId);
                } else {
                    extplaneWarning("Invalid rel command, button not held");
                }
            } else {
                INFO << "Invalid rel command";
            }
        } else if(command == "extplane-set") {
            if(subLine.size()==3) {
                if(subLine.value(1) == "update_interval") {
                    bool ok;
                    float newInterval = subLine.value(2).toFloat(&ok);
                    if(ok) {
                        emit setFlightLoopInterval(newInterval);
                    } else {
                        extplaneWarning(QString("Invalid flight loop interval %1").arg(subLine.value(2)));
                    }
                } else {
                    extplaneWarning(QString("Invalid update_interval command"));
                }
            } else {
                extplaneWarning(QString("Invalid extplane-set command"));
            }
        } else if (command == "cmd") {
            if (subLine.size()==3) {
                QString subCmd = subLine.value(1);
                extplaneCommandType type = extplaneCommandTypeInvalid;
                if (subCmd == "once") {
                    type = extplaneCommandTypeOnce;
                } else if (subCmd == "begin") {
                    type = extplaneCommandTypeBegin;
                } else if (subCmd == "end") {
                    type = extplaneCommandTypeEnd;
                } else {
                    extplaneWarning(QString("Invalid cmd command"));
                }
                QString commandName = subLine.value(2);
                m_refProvider->command(commandName, type);
            } else {
                extplaneWarning(QString("Invalid cmd command"));
            }
        } else if(command == "sit"){
            if(subLine.size() == 2) {
                m_refProvider->loadSituation(subLine.value(1));
            } else {
                extplaneWarning(QString("Invalid sit command"));
            }
        } else if(command == "fms_wpt_entry"){
            m_refProvider->addFMSEntryLatLon(subLine.value(1));
        } else if(command == "fms_clear_entries"){
            m_refProvider->clearAllFmsEntries();
        } else if(command == "fms_set_dest"){
            m_refProvider->setDestinationFmsEntry(subLine.value(1).toInt());
        } else {
            extplaneWarning(QString("Unknown command %1").arg(command));
        }
    }
}

void TcpClient::refChanged(DataRef *ref) {
    Q_ASSERT(m_subscribedRefs.find(ref) != m_subscribedRefs.end());
    Q_ASSERT(ref->isValid()); // Never send invalid values.

    // Check if the ref has changed enough to be worth sending
    if(ref->type()== extplaneRefTypeFloat) {
        FloatDataRef *refF = qobject_cast<FloatDataRef*>(ref);
        if(qAbs(refF->value() - m_refValueF[ref]) < ref->accuracy())
            return; // Hasn't changed enough
        m_refValueF.insert({ref, refF->value()});
    } else if(ref->type()== extplaneRefTypeFloatArray) {
        FloatArrayDataRef *refF = qobject_cast<FloatArrayDataRef*>(ref);
        bool bigenough = false;
        std::vector<float> values = refF->value();

        if((m_refValueFA.find(ref) == m_refValueFA.end()) || refF->accuracy() == 0 || values.size() != m_refValueFA.at(ref).size()) {
            // New value or accuracy not set.
            m_refValueFA[ref] = values;
        } else {
            auto length = values.size();
            for (unsigned long i=0; i < length; i++){
                if (qAbs(values.at(i) - m_refValueFA.at(ref).at(i)) > ref->accuracy()) {
                    bigenough = true;
                    break;
                }
            }
            if (bigenough){ // Values have changed enough
                m_refValueFA[ref] = values;
            } else {
                return;
            }
        }
    } else if(ref->type()== extplaneRefTypeIntArray) {
        IntArrayDataRef *refI = qobject_cast<IntArrayDataRef*>(ref);

        bool bigenough = false;

        auto values = refI->value();
        if((m_refValueIA.find(ref) == m_refValueIA.end()) || refI->accuracy() == 0 || values.size() != m_refValueIA.at(ref).size()) {
            // New value or accuracy not set.
            m_refValueIA.insert({ref, values});
        } else {
            auto length = values.size();

            for (unsigned long i=0; i < length; i++){
                if (qAbs(values.at(i) - m_refValueIA.at(ref).at(i)) > ref->accuracy()) {
                    bigenough = true;
                    break;
                }
            }
            if (bigenough){ // Values have changed enough
                m_refValueIA.insert({ref, values});
            } else {
                return;
            }
        }
    } else if(ref->type() == extplaneRefTypeInt) {
        IntDataRef *refI = qobject_cast<IntDataRef*>(ref);
        if(qAbs(refI->value() - m_refValueI.at(ref)) < ref->accuracy())
            return; // Hasn't changed enough
        m_refValueI.insert({ref, refI->value()});
    } else if(ref->type() == extplaneRefTypeDouble) {
        DoubleDataRef *refD = qobject_cast<DoubleDataRef*>(ref);
        if(qAbs(refD->value() - m_refValueD.at(ref)) < ref->accuracy())
            return; // Hasn't changed enough
        m_refValueD.insert({ref, refD->value()});
    } else if(ref->type() == extplaneRefTypeData) {
        // The accuracy is handled internally for the data dataref, when it emits the update signal
        // it's time to send the update...
        DataDataRef *refB = qobject_cast<DataDataRef*>(ref);
        m_refValueB.insert({ref, refB->value()});
    } else {
        extplaneWarning(QString("Ref type %1 not supported (this should not happen!)").arg(ref->type()));
        return;
    }

    // Send the ref value if we got this far..
    if(!ref->udpId())
        sendRef(ref);

    if(ref->shouldUnsubscribeAfterChange())
        unsubscribeRef(ref->name());
}

void TcpClient::sendRef(DataRef *ref) {
    Q_ASSERT(ref);
    Q_ASSERT(ref->isValid());

    QByteArray block;
    QTextStream out(&block, QIODevice::WriteOnly);
    out << "u" << ref->typeString() << " " << ref->name() << " " << ref->valueString() << "\n";
    out.flush(); // Is this optimal for performance? Should be studied.

    // INFO << QString::fromUtf8(block);
    if(m_socket->isOpen()) {
        m_socket->write(block);
    }
}

QStringList TcpClient::listRefs() {
    QStringList refNames;
    for(DataRef *ref : m_subscribedRefs)
        refNames.append(ref->name());

    return refNames;
}

DataRef *TcpClient::getSubscribedRef(const QString &name) {
    for(DataRef* r : m_subscribedRefs) {
        if(r->name() == name)
            return r;
    }
    return nullptr;
}

void TcpClient::unsubscribeRef(const QString &name) {
    DataRef *ref = getSubscribedRef(name);
    if(ref) {
        if(m_udpSender) m_udpSender->unsubscribedRef(ref);
        ref->disconnect(this);
        m_refProvider->unsubscribeRef(ref);
        m_subscribedRefs.erase(ref);
        m_refValueF.erase(ref);
        m_refValueFA.erase(ref);
        m_refValueD.erase(ref);
        m_refValueB.erase(ref);
        m_refValueI.erase(ref);
        m_refValueIA.erase(ref);
    }
}
