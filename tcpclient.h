#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QStringList>
#include <QTcpSocket>
#include <QString>
#include <QSet>
#include "dataref.h"
#include "floatdataref.h"
#include "intdataref.h"
#include "datarefprovider.h"
#include "doubledataref.h"
/**
  * Handles single client connection and tracks subscribed datarefs
  */
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
    QMap<DataRef*, double> _refValueD;
    QMap<DataRef*, float> _refValueF;
    QMap<DataRef*, int> _refValueI;
    QSet<int> _heldButtons;
    DataRefProvider *_refProvider;

};

#endif // TCPCLIENT_H
