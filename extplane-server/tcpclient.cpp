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
#include "datarefs/dataref.h"

TcpClient::TcpClient(QObject *parent,
                     QTcpSocket *socket,
                     DataRefProvider *refProvider) : QObject(parent)
  , _socket(socket)
  , _refProvider(refProvider)
{
    INFO << "Client connected from " << socket->peerAddress().toString();
    connect(_socket, SIGNAL(readyRead()), this, SLOT(readClient()));
    connect(_socket, SIGNAL(disconnected()), this, SLOT(disconnectClient()));
    connect(_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));

    QByteArray block;
    QTextStream out(&block, QIODevice::WriteOnly);
    out << "EXTPLANE " << EXTPLANE_PROTOCOL << "\n";
    out << "EXTPLANE-VERSION " << EXTPLANE_VERSION << "\n";
    out.flush();
    _socket->write(block);
}

TcpClient::~TcpClient() {
    DEBUG;
    // disconnectClient(); nope, in destructor this crashes
}

void TcpClient::socketError(QAbstractSocket::SocketError err) {
    Q_UNUSED(err)
    INFO << "Socket error:" << _socket->errorString();

    // deleteLater(); // WTF? No?
}

void TcpClient::disconnectClient() {
    INFO << Q_FUNC_INFO << this << _subscribedRefs.size();
    while(!_subscribedRefs.isEmpty()) {
        DataRef *ref = _subscribedRefs.values().first();
        _subscribedRefs.remove(ref);
        ref->disconnect(this);
        _refProvider->unsubscribeRef(ref);
    }
    for(int but : _heldButtons)
        _refProvider->buttonRelease(but);
    _socket->disconnectFromHost();
    emit discoed(this);
    deleteLater();
}

void TcpClient::extplaneWarning(QString message) {
    if(_socket->isOpen()) {
        _socket->write(QString("EXTPLANE-WARNING %1\n").arg(message).toUtf8());
    }
}

void TcpClient::readClient() {
    while(_socket->canReadLine()) {
        QByteArray lineBA = _socket->readLine();
        QString line = QString(lineBA).trimmed();

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
                    ref = _refProvider->subscribeRef(refName);
                    if(ref) { // Succesfully subscribed
                        connect(ref, &DataRef::changed, this, &TcpClient::refChanged);
                        _subscribedRefs.insert(ref);
                        ref->setAccuracy(accuracy);

                        if(ref->type() == extplaneRefTypeFloat) {
                            _refValueF[ref] = qobject_cast<FloatDataRef*>(ref)->value();
                        } else if(ref->type() == extplaneRefTypeInt) {
                            _refValueI[ref] = qobject_cast<IntDataRef*>(ref)->value();
                        } else if(ref->type() == extplaneRefTypeDouble) {
                            _refValueD[ref] = qobject_cast<DoubleDataRef*>(ref)->value();
                        } else if(ref->type() == extplaneRefTypeFloatArray) {
                            _refValueFA[ref] = qobject_cast<FloatArrayDataRef*>(ref)->value();
                        } else if(ref->type() == extplaneRefTypeIntArray) {
                            _refValueIA[ref] = qobject_cast<IntArrayDataRef*>(ref)->value();
                        } else if(ref->type() == extplaneRefTypeData) {
                            _refValueB[ref] = qobject_cast<DataDataRef*>(ref)->value();
                        }
                        INFO << "Subscribed to " << ref->name() << ", accuracy " << accuracy << ", type " << ref->typeString() << ", valid " << ref->isValid();
                        if(ref->isValid()) {
                            sendRef(ref); // Force update
                        }
                        if(command == "get") ref->setUnsubscribeAfterChange();
                    }
                } else { // Ref already subscribed - update accuracy
                    INFO << "Updating " << refName << " accuracy to " << accuracy;
                    ref->setAccuracy(accuracy);
                }
            } else {
                extplaneWarning(QString("Invalid sub command"));
            }
        } else if(command == "unsub") {
            QString refName = subLine.value(1);
            unsubscribeRef(refName);
        } else if(command == "set") {
            if(subLine.size() == 3) {
                QString refName = subLine.value(1);
                DataRef *ref = getSubscribedRef(refName);
                if (ref) {
                    if(ref->isWritable()) {
                        QString refValue = subLine.value(2).trimmed();
                        ref->setValue(refValue);
                        _refProvider->changeDataRef(ref);
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
                    _refProvider->keyStroke(keyId);
            } else {
                extplaneWarning(QString("Invalid key command"));
            }
        } else if(command == "but") {
            if(subLine.size() == 2) {
                bool ok;
                int keyId = subLine.value(1).toInt(&ok);
                if(ok) {
                    _refProvider->buttonPress(keyId);
                    _heldButtons.insert(keyId);
                }
            } else {
                extplaneWarning("Invalid but command");
            }
        } else if(command == "rel") {
            if(subLine.size() == 2) {
                bool ok;
                int keyId = subLine.value(1).toInt(&ok);
                if(ok && _heldButtons.contains(keyId)) {
                    _refProvider->buttonRelease(keyId);
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
                        emit(setFlightLoopInterval(newInterval));
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
                _refProvider->command(commandName, type);
            } else {
                extplaneWarning(QString("Invalid cmd command"));
            }
        } else if(command == "sit"){
            if(subLine.size() == 2) {
                _refProvider->loadSituation(subLine.value(1));
            } else {
                extplaneWarning(QString("Invalid sit command"));
            }
        } else if(command == "fms_wpt_entry"){
            _refProvider->addFMSEntryLatLon(subLine.value(1));
        } else if(command == "fms_clear_entries"){
            _refProvider->clearAllFmsEntries();
        } else if(command == "fms_set_dest"){
            _refProvider->setDestinationFmsEntry(subLine.value(1).toInt());
        } else {
            extplaneWarning(QString("Unknown command %1").arg(command));
        }
    }
}

void TcpClient::refChanged(DataRef *ref) {
    Q_ASSERT(_subscribedRefs.contains(ref));
    Q_ASSERT(ref->isValid()); // Never send invalid values.

    // Check if the ref has changed enough to be worth sending
    if(ref->type()== extplaneRefTypeFloat) {
        FloatDataRef *refF = qobject_cast<FloatDataRef*>(ref);
        if(qAbs(refF->value() - _refValueF[ref]) < ref->accuracy())
            return; // Hasn't changed enough
        _refValueF.insert(ref, refF->value());
    } else if(ref->type()== extplaneRefTypeFloatArray) {
        FloatArrayDataRef *refF = qobject_cast<FloatArrayDataRef*>(ref);
        bool bigenough = false;
        QVector<float> values = refF->value();

        if(_refValueFA.contains(ref) || refF->accuracy() == 0) {
            // New value or accuracy not set.
            _refValueFA.insert(ref, values);
        } else {
            long length = values.size();

            for (int i=0; i<length;i++){
                if (qAbs(values.at(i) - _refValueFA.value(ref).at(i)) > ref->accuracy()) {
                    bigenough = true;
                    break;
                }
            }
            if (bigenough){ // Values have changed enough
                _refValueFA.insert(ref, values);
            } else {
                return;
            }
        }
    } else if(ref->type()== extplaneRefTypeIntArray) {
        IntArrayDataRef *refI = qobject_cast<IntArrayDataRef*>(ref);

        bool bigenough = false;

        QVector<int> values = refI->value();
        if(_refValueIA.contains(ref) || refI->accuracy() == 0) {
            // New value or accuracy not set.
            _refValueIA.insert(ref, values);
        } else {

            long length = values.size();

            for (int i=0; i<length;i++){
                if (qAbs(values.at(i) - _refValueIA.value(ref).at(i)) > ref->accuracy()) {
                    bigenough = true;
                    break;
                }
            }
            if (bigenough){ // Values have changed enough
                _refValueIA.insert(ref, values);
            } else {
                return;
            }
        }
    } else if(ref->type() == extplaneRefTypeInt) {
        IntDataRef *refI = qobject_cast<IntDataRef*>(ref);
        if(qAbs(refI->value() - _refValueI[ref]) < ref->accuracy())
            return; // Hasn't changed enough
        _refValueI.insert(ref, refI->value());
    } else if(ref->type() == extplaneRefTypeDouble) {
        DoubleDataRef *refD = qobject_cast<DoubleDataRef*>(ref);
        if(qAbs(refD->value() - _refValueD[ref]) < ref->accuracy())
            return; // Hasn't changed enough
        _refValueD.insert(ref, refD->value());
    } else if(ref->type() == extplaneRefTypeData) {
        // The accuracy is handled internally for the data dataref, when it emits the update signal
        // it's time to send the update...
        DataDataRef *refB = qobject_cast<DataDataRef*>(ref);
        _refValueB.insert(ref, refB->value());
    } else {
        extplaneWarning(QString("Ref type %1 not supported (this should not happen!)").arg(ref->type()));
        return;
    }

    // Send the ref value if we got this far..
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
    if(_socket->isOpen()) {
        _socket->write(block);
        //    _socket->flush(); Not really needed and may mess up performance
    }
}

QStringList TcpClient::listRefs() {
    QStringList refNames;
    for(DataRef *ref : _subscribedRefs)
        refNames.append(ref->name());

    return refNames;
}

DataRef *TcpClient::getSubscribedRef(const QString &name) {
    for(DataRef* r : _subscribedRefs.values()) {
        if(r->name() == name)
            return r;
    }
    return nullptr;
}

void TcpClient::unsubscribeRef(const QString &name) {
    DataRef *ref = getSubscribedRef(name);
    if(ref) {
        ref->disconnect(this);
        _refProvider->unsubscribeRef(ref);
        _subscribedRefs.remove(ref);
        _refValueF.remove(ref);
        _refValueFA.remove(ref);
        _refValueD.remove(ref);
        _refValueB.remove(ref);
        _refValueI.remove(ref);
        _refValueIA.remove(ref);
    }
}
