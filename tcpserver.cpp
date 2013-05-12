#include "tcpserver.h"
#include "tcpclient.h"
#include "datarefprovider.h"
#include "util/console.h"

TcpServer::TcpServer(QObject *parent, DataRefProvider *refProvider) : QObject(parent), server(this), _refProvider(refProvider) {
    if(!server.listen(QHostAddress::Any, EXTPLANE_PORT)) {
        INFO << "Unable to listen on port " << EXTPLANE_PORT;
        return;
    }
    connect(&server, SIGNAL(newConnection()), this, SLOT(clientConnected()));
    INFO << "Listening on port " << EXTPLANE_PORT;
}

TcpServer::~TcpServer() {
    DEBUG;

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
    DEBUG << client;
    clientConnections.removeOne(client);
}
