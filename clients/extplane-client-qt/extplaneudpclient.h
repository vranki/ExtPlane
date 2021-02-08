#ifndef EXTPLANEUDPCLIENT_H
#define EXTPLANEUDPCLIENT_H

#include <QObject>
#include <QUdpSocket>
#include <QNetworkDatagram>
#include <map>
#include "clientdataref.h"

class ExtPlaneUdpClient : public QObject
{
    Q_OBJECT
public:
    explicit ExtPlaneUdpClient(quint8 clientId, QObject *parent = nullptr);
    bool subscribedRef(ClientDataRef *ref);
    void unsubscribedRef(ClientDataRef *ref);
private slots:
    void readPendingDatagrams();
private:
    void processDatagram(QNetworkDatagram &dg);
    void dataReceived(quint16 id, QString value);
    QUdpSocket m_socket;
    std::map<quint16, ClientDataRef*> m_idRefMap;
    quint8 m_clientId=0;
    quint16 m_udpPort = 0;
};

#endif // EXTPLANEUDPCLIENT_H
