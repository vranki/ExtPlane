#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QStringList>
#include <QTcpSocket>
#include <QString>
#include <QHostAddress>
#include <map>
#include <set>

class DataRef;
class DataRefProvider;
class UdpSender;
class TcpServer;

using namespace std;

/**
  * Handles single client connection and tracks subscribed datarefs
  */
class TcpClient : public QObject
{
    Q_OBJECT
public:
    explicit TcpClient(TcpServer *parent, QTcpSocket *socket, DataRefProvider *refProvider, quint8 clientId);
    ~TcpClient();
    QStringList listRefs();

public slots:
    void readClient();
    void refChanged(DataRef *ref);
    void socketError(QAbstractSocket::SocketError err);
    void disconnectClient(); // Disconnects this client
    void extplaneWarning(QString message);
    void flightLoop();
    void sendUdpInfo(DataRef *ref);

signals:
    void discoed(TcpClient *client);
    void setFlightLoopInterval(float newInterval);

private:
    DataRef *getSubscribedRef(const QString &name);
    void unsubscribeRef(const QString &name);
    void sendRef(DataRef *ref); // Sends the ref value to the client

    QTcpSocket *m_socket;
    set<DataRef*> m_subscribedRefs;
    map<DataRef*, double> m_refValueD;
    map<DataRef*, float> m_refValueF;
    map<DataRef*, QVector<float> > m_refValueFA;
    map<DataRef*, QVector<int> > m_refValueIA;
    map<DataRef*, int> m_refValueI;
    map<DataRef*, QByteArray> m_refValueB;
    set<int> m_heldButtons;
    DataRefProvider *m_refProvider;
    // Unsubscribe these refs after they have changed (used to implement "get" command)
    QVector<DataRef*> refsToUnsubscribeAfterChange();
    TcpServer* m_tcpserver = nullptr;
    UdpSender* m_udpSender = nullptr;
    quint8 m_clientId = 0;
};

#endif // TCPCLIENT_H
