#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QList>
#include <QTcpServer>
#include <QTcpSocket>

/**
  * Creates the TCP socket and manages client connections
  */
#define EXTPLANE_PORT 51000

class TcpClient;
class DataRefProvider;

class TcpServer : public QObject {
    Q_OBJECT
    Q_PROPERTY(int clientCount READ clientCount NOTIFY clientCountChanged)

public:
    TcpServer(QObject *parent=0, DataRefProvider *refProvider=0);
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
