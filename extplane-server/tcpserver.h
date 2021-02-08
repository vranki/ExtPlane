#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <set>
#include <QTcpServer>
#include <QTcpSocket>

#include "../protocoldefs.h"
/**
  * Creates the TCP socket and manages client connections
  */
#define EXTPLANE_STRINGIFY(s) __EXTPLANE_STRINGIFY(s)
#define __EXTPLANE_STRINGIFY(s) #s

#define EXTPLANE_PORT_STR EXTPLANE_STRINGIFY(EXTPLANE_PORT)
#define EXTPLANE_PROTOCOL_STR EXTPLANE_STRINGIFY(EXTPLANE_PROTOCOL)
#define EXTPLANE_VERSION_STR EXTPLANE_STRINGIFY(EXTPLANE_VERSION)

class TcpClient;
class DataRefProvider;

class TcpServer : public QObject {
    Q_OBJECT
    Q_PROPERTY(int clientCount READ clientCount NOTIFY clientCountChanged)

public:
    TcpServer(QObject *parent=nullptr, DataRefProvider *refProvider=nullptr);
    ~TcpServer();
    int clientCount() const;
    void setDataRefProvider(DataRefProvider *refProvider);
    quint16 reserveUdpId();
signals:
    void setFlightLoopInterval(float newInterval);
    void clientCountChanged(int clientCount);

public slots:
    void clientConnected();
    void clientDiscoed(TcpClient *client);
    void disconnectClients(); // Call before destroying
    void extplaneWarning(QString message); // Send warning message to clients
    void flightLoop();

private:
    QTcpServer server;
    std::set<TcpClient *> m_clientConnections;
    DataRefProvider *m_refProvider;
    quint16 m_nextFreeId = 0; // 0 = reserved, start from 1
    quint8 m_nextFreeClientId = 1; // Same, loops after 255 clients
};

#endif // TCPSERVER_H
