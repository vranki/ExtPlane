
#include "tcpserver.h"
#include "tcpclient.h"
#include "datarefprovider.h"
#include "console.h"

TcpServer::TcpServer(QObject *parent, DataRefProvider *refProvider) : QObject(parent)
    , server(this)
    , _refProvider(nullptr)
    , _clientCount(0) {
    connect(&server, &QTcpServer::newConnection, this, &TcpServer::clientConnected);
    setDataRefProvider(refProvider);
}

TcpServer::~TcpServer() {
    DEBUG;
    disconnectClients();
}

int TcpServer::clientCount() const {
    return _clientCount;
}

void TcpServer::setDataRefProvider(DataRefProvider *refProvider)
{
    if(_refProvider) {
        disconnectClients();
    }
    _refProvider = refProvider;

    if(_refProvider) {
        if(!server.listen(QHostAddress::Any, EXTPLANE_PORT)) {
            INFO << "Unable to listen on port " << EXTPLANE_PORT;
            return;
        }
        INFO << "Listening on port " << EXTPLANE_PORT;
    } else {
        server.close();
        INFO << "No dataref provider enabled - not listening. ";
    }
}

void TcpServer::clientConnected() {
    TcpClient *client = new TcpClient(this, server.nextPendingConnection(), _refProvider);
    connect(client, SIGNAL(discoed(TcpClient *)), this, SLOT(clientDiscoed(TcpClient *)));
    connect(client, SIGNAL(setFlightLoopInterval(float)), this, SIGNAL(setFlightLoopInterval(float)));
    clientConnections.append(client);
    _clientCount++;
    emit clientCountChanged(_clientCount);
}

void TcpServer::clientDiscoed(TcpClient *client) {
    DEBUG << client;
    clientConnections.removeOne(client);
    _clientCount--;
    emit clientCountChanged(_clientCount);
}

void TcpServer::disconnectClients()
{
    while (!clientConnections.isEmpty()) {
        TcpClient *client = clientConnections.takeLast();
        client->disconnect(this);
        client->deleteLater();
    }
}
