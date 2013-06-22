#include "tcpclient.h"
#include "datarefs/floatdataref.h"
#include "datarefs/floatarraydataref.h"
#include "datarefs/intdataref.h"
#include "datarefs/intarraydataref.h"
#include "datarefs/doubledataref.h"
#include "datarefs/datadataref.h"
#include "datarefprovider.h"
#include "util/console.h"

TcpClient::TcpClient(QObject *parent, QTcpSocket *socket, DataRefProvider *refProvider) :
        QObject(parent), _socket(socket), _refProvider(refProvider)
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
    _socket->disconnectFromHost();
    while(!_subscribedRefs.isEmpty()) {
        DataRef *ref = _subscribedRefs.values().first();
        _subscribedRefs.remove(ref);
        ref->disconnect(this);
        _refProvider->unsubscribeRef(ref);
    }
    foreach(int but, _heldButtons)
        _refProvider->buttonRelease(but);
    emit discoed(this);
}

void TcpClient::socketError(QAbstractSocket::SocketError err) {
    INFO << "Socket error:" << _socket->errorString();
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
            deleteLater();
        } else if(command == "sub") { // Subscribe command
            if(subLine.length() >= 2) {
                QString refName = subLine[1].trimmed();

                double accuracy = 0;
                if(subLine.length() >=3)
                    accuracy = subLine[2].toDouble();

                DataRef *ref = getSubscribedRef(refName);
                if(!ref) { // Ref not subscribed yet, try to subscribe
                    ref = _refProvider->subscribeRef(refName);
                    if(ref) { // Succesfully subscribed
                        connect(ref, SIGNAL(changed(DataRef*)), this, SLOT(refChanged(DataRef*)));
                        _subscribedRefs.insert(ref);
                        ref->setAccuracy(accuracy);
                        //TODO: why is ref->updateValue() not sufficient here?
                        if(ref->type() == xplmType_Float) {
                            _refValueF[ref] = qobject_cast<FloatDataRef*>(ref)->value();
                        } else if(ref->type() == xplmType_Int) {
                            _refValueI[ref] = qobject_cast<IntDataRef*>(ref)->value();
                        } else if(ref->type() == xplmType_Double) {
                            _refValueD[ref] = qobject_cast<DoubleDataRef*>(ref)->value();
                        } else if(ref->type() == xplmType_FloatArray) {
                            _refValueFA[ref] = qobject_cast<FloatArrayDataRef*>(ref)->value();
                        } else if(ref->type() == xplmType_IntArray) {
                            _refValueIA[ref] = qobject_cast<IntArrayDataRef*>(ref)->value();
                        } else if(ref->type() == xplmType_Data) {
                            _refValueB[ref] = qobject_cast<DataDataRef*>(ref)->value();
                        }
                        INFO << "Subscribed to " << ref->name() << ", accuracy " << accuracy << ", type " << ref->typeString();
                    } else {
                        INFO << "Ref not found" << refName;
                    }
                } else { // Ref already subscribed - update accuracy
                    INFO << "Updating " << refName << " accuracy to " << accuracy;
                    ref->setAccuracy(accuracy);
                    ref->updateValue();
                }
            } else {
                INFO << "Invalid sub command";
            }
        } else if(command == "unsub") {
            QString refName = subLine.value(1);
            DataRef *ref = getSubscribedRef(refName);
            if(ref) {
                ref->disconnect(this);
                _refProvider->unsubscribeRef(ref);
                _subscribedRefs.remove(ref);
                _refValueF.remove(ref);
                _refValueFA.remove(ref);
                _refValueB.remove(ref);
                _refValueI.remove(ref);
                _refValueIA.remove(ref);
            }
        } else if(command == "set") {
            if(subLine.size() == 3) {
                QString refName = subLine.value(1);
                QString refValue = subLine.value(2).trimmed();
                DataRef *ref = getSubscribedRef(refName);
                if (ref) {
                    if(ref->isWritable()) {
                        ref->setValue(refValue);
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

        } else {
            INFO << "Unknown command " << command;
        }
    }
}

void TcpClient::refChanged(DataRef *ref) {
    Q_ASSERT(_subscribedRefs.contains(ref));
    if(ref->type()== xplmType_Float) {
        FloatDataRef *refF = qobject_cast<FloatDataRef*>(ref);
        if(qAbs(refF->value() - _refValueF[ref]) < ref->accuracy())
            return; // Hasn't changed enough
        _refValueF[ref] = refF->value();
    } else if(ref->type()== xplmType_FloatArray) {
        FloatArrayDataRef *refF = qobject_cast<FloatArrayDataRef*>(ref);
        
        bool bigenough = false;

        QVector<float> values = refF->value();
        long length = values.size();
        
        for (int i=0; i<length;i++){
            if (qAbs(values[i] - _refValueFA[ref][i]) > ref->accuracy()) {
                bigenough = true;
                break;
            }
        }
        if (bigenough){ // Values have changed enough
            for (int i=0; i<length;i++){
                _refValueFA[ref][i] = values[i];
            }    
        } else {
            return;
        }
    } else if(ref->type()== xplmType_IntArray) {
        IntArrayDataRef *refF = qobject_cast<IntArrayDataRef*>(ref);

        bool bigenough = false;

        QVector<int> values = refF->value();
        long length = values.size();

        for (int i=0; i<length;i++){
            if (qAbs(values[i] - _refValueIA[ref][i]) > ref->accuracy()) {
                bigenough = true;
                break;
            }
        }
        if (bigenough){ // Values have changed enough
            for (int i=0; i<length;i++){
                _refValueIA[ref][i] = values[i];
            }
        } else {
            return;
        }
    } else if(ref->type()== xplmType_Int) {
        IntDataRef *refI = qobject_cast<IntDataRef*>(ref);
        if(qAbs(refI->value() - _refValueI[ref]) < ref->accuracy())
            return; // Hasn't changed enough
        _refValueI[ref] = refI->value();
    } else if(ref->type()== xplmType_Double) {
        DoubleDataRef *refD = qobject_cast<DoubleDataRef*>(ref);
        if(qAbs(refD->value() - _refValueD[ref]) < ref->accuracy())
            return; // Hasn't changed enough
        _refValueD[ref] = refD->value();
    } else if(ref->type()== xplmType_Data) {
        DataDataRef *refB = qobject_cast<DataDataRef*>(ref);
        _refValueB[ref] = refB->value();
    } else {
        INFO << "Ref type " << ref->type() << " not supported (this should not happen!)";
        return;
    }
    QByteArray block;
    QTextStream out(&block, QIODevice::WriteOnly);
    out << "u" << ref->typeString() << " " << ref->name() << " " << ref->valueString() << "\n";
    out.flush();

    if(_socket->isOpen()) {
        _socket->write(block);
        //    _socket->flush(); Not really needed and may mess up performance
    }
}

QSet<QString> TcpClient::listRefs() {
    QSet<QString> refNames;
    foreach(DataRef *ref, _subscribedRefs)
        refNames.insert(ref->name());

    return refNames;
}
DataRef *TcpClient::getSubscribedRef(QString name) {
    foreach(DataRef* r, _subscribedRefs.values()) {
        if(r->name()==name)
            return r;
    }
    return 0;
}
