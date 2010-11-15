#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QList>
#include "dataref.h"
#include "tcpclient.h"
#include "datarefprovider.h"

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
