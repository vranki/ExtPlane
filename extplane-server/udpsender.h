#ifndef UDPSENDER_H
#define UDPSENDER_H

#include <QObject>
#include <QUdpSocket>
#include <QDataStream>
#include <set>

class DataRef;
class DataRefProvider;
class FloatDataRef;
class DoubleDataRef;
class IntDataRef;

#define UDP_PACKET_SIZE 508
#define BUFFER_RESERVE 4096 // Size to resize empty buffers

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
    DataRefProvider *m_refProvider = nullptr;

    QUdpSocket m_socket;
    QHostAddress m_host;
    quint8 m_clientId = 0;

    QByteArray m_bufInt;
    quint16 m_countInt = 0;
    QByteArray m_bufFloat;
    quint16 m_countFloat = 0;
    QByteArray m_bufDouble;
    quint16 m_countDouble = 0;
    quint16 m_udpPort = 0;
};

#endif // UDPSENDER_H
