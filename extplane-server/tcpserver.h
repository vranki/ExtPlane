#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QList>
#include <QTcpServer>
#include <QTcpSocket>

/**
  * Creates the TCP socket and manages client connections
  */
// TCP port used to listen for connections
#define EXTPLANE_PORT 51000
// Network protocol, currently always 1
#define EXTPLANE_PROTOCOL 1
// Feature revision, every time we add a new feature or bug fix, this should be incremented so that clients can know how old the plugin is
#define EXTPLANE_VERSION 1000

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
    TcpServer(QObject *parent=nullptr, DataRefProvider *refProvider=0);
    ~TcpServer();
    int clientCount() const;
    void setDataRefProvider(DataRefProvider *refProvider);

signals:
    void setFlightLoopInterval(float newInterval);
    void clientCountChanged(int clientCount);

public slots:
    void clientConnected();
    void clientDiscoed(TcpClient *client);
private:
    void disconnectClients();
    QTcpServer server;
    QList<TcpClient *> clientConnections;
    DataRefProvider *_refProvider;
    int _clientCount;
};

#endif // TCPSERVER_H
