#include "tcpserver.h"
#include "tcpclient.h"
#include "datarefprovider.h"

TcpServer::TcpServer(QObject *parent, DataRefProvider *refProvider) : QObject(parent), server(this), _refProvider(refProvider) {
    if(!server.listen(QHostAddress::Any, EXTPLANE_PORT)) {
        qDebug() << Q_FUNC_INFO << "Unable to listen on port " << EXTPLANE_PORT;
        return;
    }
    connect(&server, SIGNAL(newConnection()), this, SLOT(clientConnected()));
    qDebug() <<  Q_FUNC_INFO << "Listening on port " << EXTPLANE_PORT;
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
    connect(client, SIGNAL(setFlightLoopInterval(float)), this, SIGNAL(setFlightLoopInterval(float)));
    clientConnections.append(client);
}

void TcpServer::clientDiscoed(TcpClient *client) {
    qDebug() << Q_FUNC_INFO << client;
    clientConnections.removeOne(client);
}
