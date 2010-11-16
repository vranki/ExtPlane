#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QList>
#include "dataref.h"
#include "tcpclient.h"
#include "datarefprovider.h"
/**
  * Creates the TCP socket and manages client connections
  */
#define EXTPLANE_PORT 51000

class TcpServer : public QObject {
    Q_OBJECT
public:
    TcpServer(QObject *parent, DataRefProvider *refProvider);
   ~TcpServer();
public slots:
     void clientConnected();
     void clientDiscoed(TcpClient *client);
private:
    QTcpServer server;
    QList<TcpClient *> clientConnections;
    DataRefProvider *_refProvider;
};

#endif // TCPSERVER_H
