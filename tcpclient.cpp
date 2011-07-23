#include "tcpclient.h"

TcpClient::TcpClient(QObject *parent, QTcpSocket *socket, DataRefProvider *refProvider) :
        QObject(parent), _socket(socket), _refProvider(refProvider)
{
    qDebug() << Q_FUNC_INFO << " client connected from " << socket->peerAddress();
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
    qDebug() << Q_FUNC_INFO;
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
    qDebug() << Q_FUNC_INFO << _socket->errorString();
    deleteLater();
}

void TcpClient::readClient() {
    while(_socket->canReadLine()) {
        QByteArray lineBA = _socket->readLine();

        QString line = QString(lineBA);
        qDebug() << Q_FUNC_INFO << "Client says: " << line;
        QStringList subLine = line.split(" ", QString::SkipEmptyParts);
        QString command = subLine.value(0);
        if(command == "disconnect") {
            qDebug() << Q_FUNC_INFO << "killing this client connection";
            deleteLater();
        } else if(command == "sub") {
            if(subLine.length() >= 2) {
                QString refName = subLine[1].trimmed();

                double accuracy = 0;
                if(subLine.length() >=3)
                    accuracy = subLine[2].toDouble();

                DataRef *ref = getSubscribedRef(refName);
                if(!ref) {
                    ref = _refProvider->subscribeRef(refName);
                    if(ref) {
                        connect(ref, SIGNAL(changed(DataRef*)), this, SLOT(refChanged(DataRef*)));
                        _subscribedRefs.insert(ref);
                        _refAccuracy[ref] = accuracy;
                        if(ref->type() == xplmType_Float) {
                            _refValueF[ref] = qobject_cast<FloatDataRef*>(ref)->value();
                        } else if(ref->type() == xplmType_Int) {
                            _refValueI[ref] = qobject_cast<IntDataRef*>(ref)->value();
                        } else if(ref->type() == xplmType_Double) {
                            _refValueD[ref] = qobject_cast<DoubleDataRef*>(ref)->value();
                        } else if(ref->type() == xplmType_FloatArray) {
                            float *value = qobject_cast<FloatArrayDataRef*>(ref)->value();
                            long length = (long) value[0];
                            _refValueFA[ref] = (float *) calloc(length, sizeof(float));
                            for (int i=0;i<length;i++){
                                _refValueFA[ref][i]=value[i+1];
                            }
                        }
                        qDebug() << Q_FUNC_INFO << "Subscribed to " << ref->name() << ", accuracy " << accuracy;
                    } else {
                        qDebug() << Q_FUNC_INFO << "Ref not found" << refName;
                    }
                } else {
                    qDebug() << Q_FUNC_INFO << "Updating " << refName << " accuracy to " << accuracy;
                    _refAccuracy[ref] = accuracy;
                }
            } else {
                qDebug() << Q_FUNC_INFO << "Invalid sub command";
            }
        } else if(command == "unsub") {
            QString refName = subLine.value(1);
            DataRef *ref = getSubscribedRef(refName);
            if(ref) {
                ref->disconnect(this);
                _refProvider->unsubscribeRef(ref);
                _subscribedRefs.remove(ref);
                _refAccuracy.remove(ref);
                _refValueF.remove(ref);
            }
        } else if(command == "set") {
            if(subLine.size() == 3) {
                QString refName = subLine.value(1);
                QString refValue = subLine.value(2);
                DataRef *ref = getSubscribedRef(refName);
                if (ref) {
                    ref->setValue(refValue);
                }
            } else {
                qDebug() << Q_FUNC_INFO << "Invalid set command";
            }
        } else if(command == "key") {
            if(subLine.size() == 2) {
                bool ok;
                int keyId = subLine.value(1).toInt(&ok);
                if(ok)
                    _refProvider->keyStroke(keyId);
            } else {
                qDebug() << Q_FUNC_INFO << "Invalid key command";
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
                qDebug() << Q_FUNC_INFO << "Invalid but command";
            }
        } else if(command == "rel") {
            if(subLine.size() == 2) {
                bool ok;
                int keyId = subLine.value(1).toInt(&ok);
                if(ok && _heldButtons.contains(keyId)) {
                    _refProvider->buttonRelease(keyId);
                } else {
                    qDebug() << Q_FUNC_INFO << "Invalid rel command, button not held";
                }
            } else {
                qDebug() << Q_FUNC_INFO << "Invalid rel command";
            }
        }
    }
}

void TcpClient::refChanged(DataRef *ref) {
    Q_ASSERT(_subscribedRefs.contains(ref));
    if(ref->type()== xplmType_Float) {
        FloatDataRef *refF = qobject_cast<FloatDataRef*>(ref);
        if(qAbs(refF->value() - _refValueF[ref]) < _refAccuracy[ref])
            return; // Hasn't changed enough
        _refValueF[ref] = refF->value();
    } else if(ref->type()== xplmType_FloatArray) {                          // BB needs thought...
        FloatArrayDataRef *refF = qobject_cast<FloatArrayDataRef*>(ref);
        
        bool bigenough = false;
        float * value = refF->value();
        long length = (long) value[0];
        
        for (int i=0; i<length;i++){
            if (qAbs(value[i+1] - _refValueFA[ref][i]) > _refAccuracy[ref]) bigenough = true;
        }
        if (bigenough){
            for (int i=0; i<length;i++){
                _refValueFA[ref][i] = value[i+1];
            }    
        } else {
            return;
        }
    } else if(ref->type()== xplmType_Int) {
        IntDataRef *refI = qobject_cast<IntDataRef*>(ref);
        if(qAbs(refI->value() - _refValueI[ref]) < _refAccuracy[ref])
            return; // Hasn't changed enough
        _refValueI[ref] = refI->value();
    } else if(ref->type()== xplmType_Double) {
        DoubleDataRef *refD = qobject_cast<DoubleDataRef*>(ref);
        if(qAbs(refD->value() - _refValueD[ref]) < _refAccuracy[ref])
            return; // Hasn't changed enough
        _refValueD[ref] = refD->value();
    } else {
        qDebug( ) << Q_FUNC_INFO << "Ref type " << ref->type() << " not supported (this should not happen!)";
        return;
    }
    QByteArray block;
    QTextStream out(&block, QIODevice::WriteOnly);
    out << "u" << ref->typeString() << " " << ref->name() << " " << ref->valueString() << "\n";
    out.flush();

    if(_socket->isOpen()) {
        _socket->write(block);
        _socket->flush();
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
