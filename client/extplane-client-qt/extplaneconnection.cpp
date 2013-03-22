#include <QDebug>
#include "extplaneconnection.h"
#include "clientdataref.h"
#include "simulateddatarefs/simulateddataref.h"
#include "extplaneclient.h"

ExtPlaneConnection::ExtPlaneConnection(QObject *parent) : QTcpSocket(parent), updateInterval(0.333) {
    connect(this, SIGNAL(connected()), this, SLOT(socketConnected()));
    connect(this, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));
    connect(this, SIGNAL(readyRead()), this, SLOT(readClient()));
    connect(&reconnectTimer, SIGNAL(timeout()), this, SLOT(tryReconnect()));
    server_ok = false;
    enableSimulatedRefs = false;
}

void ExtPlaneConnection::connectTo(QHostAddress addr, unsigned int port) {
    close();
    _addr = addr;
    _port = port;
    abort();
    tryReconnect();
}

void ExtPlaneConnection::setUpdateInterval(double newInterval) {
    updateInterval = newInterval;
    if(server_ok & updateInterval > 0) {
        QString line = "extplane-set update_interval " + QString::number(newInterval);
        writeLine(line);
    }
}

void ExtPlaneConnection::tryReconnect() {
    emit connectionMessage(QString("Connecting to ExtPlane at %1:%2..").arg(_addr.toString()).arg(_port));

    reconnectTimer.stop();
    connectToHost(_addr, _port);
}

void ExtPlaneConnection::socketConnected() {
    emit connectionMessage("Connected to ExtPlane, waiting for handshake");
    reconnectTimer.stop();
}

void ExtPlaneConnection::socketError(QAbstractSocket::SocketError err) {
    qDebug() << Q_FUNC_INFO << errorString();
    server_ok = false;
    emit connectionMessage(errorString() + " : " + peerName() + ":" + QString::number(peerPort()));
    reconnectTimer.setInterval(5000);
    reconnectTimer.start();
}

void ExtPlaneConnection::registerClient(ExtPlaneClient* client) {
    clients.append(client);
}

ClientDataRef *ExtPlaneConnection::subscribeDataRef(QString name, double accuracy) {
    ClientDataRef *ref = dataRefs.value(name);
    if(ref){
        qDebug() << Q_FUNC_INFO << "Ref already subscribed";
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

    qDebug() << Q_FUNC_INFO << name << ref->subscribers() << server_ok;
    return ref;
}


ClientDataRef *ExtPlaneConnection::createDataRef(QString name, double accuracy) {
    return new ClientDataRef(this, name, accuracy);
}

void ExtPlaneConnection::unsubscribeDataRef(ClientDataRef *ref) {
    qDebug() << Q_FUNC_INFO << ref << ref->name() << ref->subscribers();
    ref->setSubscribers(ref->subscribers() - 1);
    if(ref->subscribers() > 0) return;
    qDebug() << Q_FUNC_INFO << "Ref not subscribed by anyone anymore";
    dataRefs.remove(ref->name());
    bool isSimulated = false;
    foreach(SimulatedDataRef *simRef, simulatedRefs) {
        if(simRef->clientRef()==ref) {
            simRef->deleteLater();
            simulatedRefs.removeOne(simRef);
            isSimulated = true;
        }
    }
    disconnect(ref, 0, this, 0);
    if(!isSimulated) {
        if(server_ok)
            writeLine("unsub " + ref->name());
        qDebug() << Q_FUNC_INFO << "Deleting ref " << ref->name() << ref;
        ref->deleteLater();
    }
    foreach(ClientDataRef *ref, dataRefs) {
        qDebug() << "refs now:" << ref->name();
    }
}

void ExtPlaneConnection::readClient() {
    while(canReadLine()) {
        QByteArray lineBA = readLine();
        QString line = QString(lineBA).trimmed();
        qDebug() << Q_FUNC_INFO << "Server says: " << line;
        if(!server_ok) { // Waiting for handshake..
            if(line=="EXTPLANE 1") {
                server_ok = true;
                emit connectionMessage("");
                setUpdateInterval(updateInterval);
                // Sub all refs
                foreach(ClientDataRef *ref, dataRefs)
                    subRef(ref);
            }
            return;
        } else { // Handle updates
            QStringList cmd = line.split(" ", QString::SkipEmptyParts);
            if(cmd.size()==3) {
                ClientDataRef *ref = dataRefs.value(cmd.value(1));
                if(ref) {
                    if (cmd.value(0)=="ufa" || cmd.value(0)=="uia"){ // Array dataref
                        QString arrayString = cmd.value(2);
                        Q_ASSERT(arrayString[0]=='[' && arrayString[arrayString.length()-1]==']');
                        arrayString = arrayString.mid(1, arrayString.length()-2);
                        QStringList arrayValues = arrayString.split(',');
                        ref->updateValue(arrayValues);
                    } else if ((cmd.value(0)=="uf")||(cmd.value(0)=="ui")) { // Single value dataref
                        ref->updateValue(cmd.value(2));
                    } else {
                        qDebug() << Q_FUNC_INFO << "unsupported ref type " << cmd.value(0);
                    }
                } else {
                    qDebug() << Q_FUNC_INFO << "ref not subscribed " << cmd.value(2);
                }
            }
        }
    }
}

void ExtPlaneConnection::subRef(ClientDataRef *ref) {
    qDebug() << Q_FUNC_INFO;
    Q_ASSERT(server_ok);
    QString subLine = "sub " + ref->name();
    if(ref->accuracy() != 0) {
        subLine += " " + QString::number(ref->accuracy());
    }
    writeLine(subLine);
}

void ExtPlaneConnection::writeLine(QString line) {
    if(!server_ok) {
        qDebug() << Q_FUNC_INFO << "Connection not ok! Tried to write" << line;
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
        setValue(ref->name(), ref->valueString());
    } else {
        setValues(ref->name(), ref->valueStrings());
    }
}

void ExtPlaneConnection::tickTime(double dt, int total) {
    foreach(SimulatedDataRef *dr, simulatedRefs)
        dr->tickTime(dt, total);
}
