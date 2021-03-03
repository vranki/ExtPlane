#ifndef UDPSENDER_H
#define UDPSENDER_H

#include <QObject>
#include <QUdpSocket>
#include <set>

class DataRef;
class DataRefProvider;

class UdpSender : public QObject
{
    Q_OBJECT
public:
    explicit UdpSender(DataRefProvider *refProvider, QHostAddress host, quint8 clientId, QObject *parent = nullptr);
    bool subscribedRef(DataRef *ref);
    void unsubscribedRef(DataRef *ref);
    void flightLoop();

private slots:
    void refChanged(DataRef *ref);

private:
    quint16 countRefs(int type);
    DataRefProvider *m_refProvider = nullptr;

    QUdpSocket m_socket;
    QHostAddress m_host;
    quint8 m_clientId = 0;
    quint16 m_udpPort = 0;
    std::set<DataRef *> m_changedRefs; // List of refs changed since last send
};

#endif // UDPSENDER_H
