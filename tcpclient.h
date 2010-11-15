#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QStringList>
#include <QTcpSocket>
#include "dataref.h"
#include "datarefprovider.h"

class TcpClient : public QObject
{
    Q_OBJECT
public:
    explicit TcpClient(QObject *parent, QTcpSocket *socket, DataRefProvider *refProvider);
    ~TcpClient();
    QSet<QString> listRefs();
public slots:
    void readClient();
    void refChanged(DataRef *ref);
signals:
    void discoed(TcpClient *client);
private:
    DataRef *getSubscribedRef(QString name);
    QTcpSocket *_socket;
    QSet<DataRef*> _subscribedRefs;
    QMap<DataRef*, double> _refAccuracy;
    QMap<DataRef*, double> _refValue;
    DataRefProvider *_refProvider;
};

#endif // TCPCLIENT_H
