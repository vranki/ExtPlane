
#include "tcpserver.h"
#include "tcpclient.h"
#include "datarefprovider.h"
#include "console.h"

TcpServer::TcpServer(QObject *parent, DataRefProvider *refProvider)
    : QObject(parent)
      , m_refProvider(nullptr) {
    connect(&server, &QTcpServer::newConnection, this, &TcpServer::clientConnected);
    setDataRefProvider(refProvider);
}

TcpServer::~TcpServer() {
    DEBUG;
    disconnectClients();
}

int TcpServer::clientCount() const {
    return m_clientConnections.size();
}

void TcpServer::setDataRefProvider(DataRefProvider *refProvider) {
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

quint16 TcpServer::reserveUdpId() {
    m_nextFreeId++;
    // TODO: Recalc list if this happens
    Q_ASSERT(m_nextFreeId < 65535);
    return m_nextFreeId;
}

void TcpServer::clientConnected() {
    TcpClient *client = new TcpClient(this, server.nextPendingConnection(), m_refProvider, m_nextFreeClientId);
    m_nextFreeClientId++;
    connect(client, &TcpClient::discoed, this, &TcpServer::clientDiscoed);
    connect(client, &TcpClient::setFlightLoopInterval, this, &TcpServer::setFlightLoopInterval);
    m_clientConnections.insert(client);
    emit clientCountChanged(clientCount());
}

void TcpServer::clientDiscoed(TcpClient *client) {
    DEBUG << client;
    m_clientConnections.erase(client);
    emit clientCountChanged(clientCount());
}

void TcpServer::disconnectClients() {
    while (!m_clientConnections.empty()) {
        TcpClient *client = *m_clientConnections.begin();
        client->disconnectClient(); // Will cause clientDiscoed() signal and deletelater this client
    }
}

void TcpServer::extplaneWarning(QString message) {
    INFO << "Sending extplane warning " << message << "to" << m_clientConnections.size() << "clients";
    for(auto cc : m_clientConnections) {
        cc->extplaneWarning(message);
    }
}

void TcpServer::flightLoop() {
    for(auto cc : m_clientConnections) {
        cc->flightLoop();
    }
}
