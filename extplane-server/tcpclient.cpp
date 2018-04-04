#include "tcpclient.h"
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
    connect(_socket, SIGNAL(disconnected()), this, SLOT(deleteLater()));
    connect(_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));

    QByteArray block;
    QTextStream out(&block, QIODevice::WriteOnly);
    out << "EXTPLANE 1\n";
    out.flush();
    _socket->write(block);
}

TcpClient::~TcpClient() {
    DEBUG;
    disconnectClient();
}

void TcpClient::socketError(QAbstractSocket::SocketError err) {
    Q_UNUSED(err);
    INFO << "Socket error:" << _socket->errorString();
    deleteLater();
}

void TcpClient::disconnectClient() {
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

void TcpClient::readClient() {
    while(_socket->canReadLine()) {
        QByteArray lineBA = _socket->readLine();

        QString line = QString(lineBA).trimmed();
        DEBUG << "Client says: " << line;
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
                if(subLine.length() >=3)
                    accuracy = subLine[2].toDouble();

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
                        INFO << "Subscribed to " << ref->name() << ", accuracy " << accuracy << ", type " << ref->typeString();
                        if(ref->isValid()) {
                            emit ref->changed(ref); // Force update to all clients
                        }
                        if(command == "get") ref->setUnsubscribeAfterChange();
                    } else {
                        INFO << "Ref not found" << refName;
                    }
                } else { // Ref already subscribed - update accuracy
                    INFO << "Updating " << refName << " accuracy to " << accuracy;
                    ref->setAccuracy(accuracy);
                }
            } else {
                INFO << "Invalid sub command";
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
                        INFO << "Ref " << ref->name() << " is not writable!";
                    }
                }
            } else {
                INFO << "Invalid set command";
            }
        } else if(command == "key") {
            if(subLine.size() == 2) {
                bool ok;
                int keyId = subLine.value(1).toInt(&ok);
                if(ok)
                    _refProvider->keyStroke(keyId);
            } else {
                INFO << "Invalid key command";
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
                INFO << "Invalid but command";
            }
        } else if(command == "rel") {
            if(subLine.size() == 2) {
                bool ok;
                int keyId = subLine.value(1).toInt(&ok);
                if(ok && _heldButtons.contains(keyId)) {
                    _refProvider->buttonRelease(keyId);
                } else {
                    INFO << "Invalid rel command, button not held";
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
                        INFO << "Invalid interval";
                    }
                } else {
                    INFO << "Invalid update_interval command";
                }
            } else {
                INFO << "Invalid extplane-set command";
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
                    INFO << "Invalid cmd command";
                }
                QString commandName = subLine.value(2);
                _refProvider->command(commandName, type);
            } else {
                INFO << "Invalid cmd command";
            }
        } else if(command == "sit"){
            if(subLine.size() == 2) {
               _refProvider->loadSituation(subLine.value(1));
            } else {
                INFO << "Invalid sit command";
            }
        } else {
            INFO << "Unknown command " << command;
        }
    }
}

void TcpClient::refChanged(DataRef *ref) {
    Q_ASSERT(_subscribedRefs.contains(ref));
    Q_ASSERT(ref->isValid()); // Never send invalid values.

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
        INFO << "Ref type " << ref->type() << " not supported (this should not happen!)";
        return;
    }
    QByteArray block;
    QTextStream out(&block, QIODevice::WriteOnly);
    out << "u" << ref->typeString() << " " << ref->name() << " " << ref->valueString() << "\n";
    out.flush(); // Is this optimal for performance? Should be studied.

    if(_socket->isOpen()) {
        _socket->write(block);
        //    _socket->flush(); Not really needed and may mess up performance
    }
    if(ref->shouldUnsubscribeAfterChange())
        unsubscribeRef(ref->name());
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
