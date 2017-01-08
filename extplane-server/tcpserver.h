#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QList>
#include <QTcpServer>
#include <QTcpSocket>

#include "datarefs/dataref.h"
/**
  * Creates the TCP socket and manages client connections
  */
#define EXTPLANE_PORT 51000

class TcpClient;
class DataRefProvider;

class TcpServer : public QObject {
    Q_OBJECT
public:
    TcpServer(QObject *parent, DataRefProvider *refProvider);
   ~TcpServer();
signals:
    void setFlightLoopInterval(float newInterval);
public slots:
     void clientConnected();
     void clientDiscoed(TcpClient *client);
private:
    QTcpServer server;
    QList<TcpClient *> clientConnections;
    DataRefProvider *_refProvider;
};

#endif // TCPSERVER_H
