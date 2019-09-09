
#include "tcpserver.h"
#include "tcpclient.h"
#include "datarefprovider.h"
#include "console.h"

TcpServer::TcpServer(QObject *parent, DataRefProvider *refProvider) : QObject(parent)
                                                                      , m_refProvider(nullptr) {
    connect(&server, &QTcpServer::newConnection, this, &TcpServer::clientConnected);
    setDataRefProvider(refProvider);
}

TcpServer::~TcpServer() {
    DEBUG;
    disconnectClients();
}

int TcpServer::clientCount() const {
    return m_clientConnections.length();
}

void TcpServer::setDataRefProvider(DataRefProvider *refProvider)
{
    if(m_refProvider) {
        disconnectClients();
    }
    m_refProvider = refProvider;

    if(m_refProvider) {
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
    TcpClient *client = new TcpClient(this, server.nextPendingConnection(), m_refProvider);
    connect(client, &TcpClient::discoed, this, &TcpServer::clientDiscoed);
    connect(client, &TcpClient::setFlightLoopInterval, this, &TcpServer::setFlightLoopInterval);
    m_clientConnections.append(client);
    emit clientCountChanged(clientCount());
}

void TcpServer::clientDiscoed(TcpClient *client) {
    DEBUG << client;
    m_clientConnections.removeOne(client);
    emit clientCountChanged(clientCount());
}

void TcpServer::disconnectClients() {
    INFO << Q_FUNC_INFO << m_clientConnections.length();
    while (!m_clientConnections.isEmpty()) {
        TcpClient *client = m_clientConnections.first();
        INFO << Q_FUNC_INFO << "Discoing" << client << "length" << m_clientConnections.length();
        client->disconnectClient(); // Will cause clientDiscoed() signal and deletelater this client
    }
}
