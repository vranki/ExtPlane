#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QStringList>
#include <QTcpSocket>
#include <QString>
#include <QSet>
#include <QVector>
#include <QHostAddress>

class DataRef;
class DataRefProvider;

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
    void socketError(QAbstractSocket::SocketError err);
signals:
    void discoed(TcpClient *client);
    void setFlightLoopInterval(float newInterval);
private:
    DataRef *getSubscribedRef(QString name);
    QTcpSocket *_socket;
    QSet<DataRef*> _subscribedRefs;
    QMap<DataRef*, double> _refAccuracy;
    QMap<DataRef*, double> _refValueD;
    QMap<DataRef*, float> _refValueF;
    QMap<DataRef*, QVector<float> > _refValueFA;
    QMap<DataRef*, QVector<int> > _refValueIA;
    QMap<DataRef*, int> _refValueI;
    QSet<int> _heldButtons;
    DataRefProvider *_refProvider;

};

#endif // TCPCLIENT_H
