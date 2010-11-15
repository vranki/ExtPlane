#include "tcpserver.h"

TcpServer::TcpServer(QObject *parent, DataRefProvider *refProvider) : QObject(parent), server(this), _refProvider(refProvider) {
    if(!server.listen(QHostAddress::Any, 51000)) {
        qDebug() << Q_FUNC_INFO << "Unable to listen";
        return;
    }
    connect(&server, SIGNAL(newConnection()), this, SLOT(clientConnected()));
    qDebug() <<  Q_FUNC_INFO << "Listening on port 51000";
}

TcpServer::~TcpServer() {
    qDebug() << Q_FUNC_INFO;

    while (!clientConnections.isEmpty()) {
        TcpClient *client = clientConnections.takeLast();
        client->disconnect(this);
        delete client;
    }
}

void TcpServer::clientConnected() {
    TcpClient *client = new TcpClient(this, server.nextPendingConnection(), _refProvider);
    connect(client, SIGNAL(discoed(TcpClient *)), this, SLOT(clientDiscoed(TcpClient *)));
    //    connect(client, SIGNAL(subscribeRef(QString)), this, SIGNAL(subscribeRef(QString)));
    //    connect(client, SIGNAL(unsubscribeRef(QString)), this, SLOT(checkIfRefCanBeUnsubscribed(QString)));
    clientConnections.append(client);
}

void TcpServer::clientDiscoed(TcpClient *client) {
    qDebug() << Q_FUNC_INFO << client;
    clientConnections.removeOne(client);
}
