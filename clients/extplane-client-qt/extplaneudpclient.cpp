#include "extplaneudpclient.h"
#include <QDataStream>
#include "../../protocoldefs.h"
#include "../../util/console.h"

ExtPlaneUdpClient::ExtPlaneUdpClient(quint8 clientId, QObject *parent) : QObject(parent)
  , m_clientId(clientId)
{
    m_udpPort = UDP_OUT_PORT_BASE + m_clientId;
    if(m_socket.bind(m_udpPort)) {
        connect(&m_socket, &QUdpSocket::readyRead, this, &ExtPlaneUdpClient::readPendingDatagrams);
        INFO << "UDP listener bound to port" << m_udpPort;
    } else {
        qWarning() << Q_FUNC_INFO << "Failed to bind socket to port" << m_udpPort;
    }
}

bool ExtPlaneUdpClient::subscribedRef(ClientDataRef *ref) {
    m_idRefMap.insert({ref->udpId(), ref});
    return true;
}

void ExtPlaneUdpClient::unsubscribedRef(ClientDataRef *ref) {
    ref->setUdpId(0);
    m_idRefMap.erase(ref->udpId());
}

void ExtPlaneUdpClient::readPendingDatagrams()
{
    while(m_socket.hasPendingDatagrams()) {
        QNetworkDatagram dg = m_socket.receiveDatagram();
        processDatagram(dg);
    }
}

void ExtPlaneUdpClient::processDatagram(QNetworkDatagram &dg)
{
    QByteArray data = dg.data();
    QDataStream s(&data, QIODevice::ReadOnly);
    if(!data.startsWith("EXTP_")) {
        qWarning() << Q_FUNC_INFO << "Wrong header, not extplane packet??";
        return;
    }
    s.skipRawData(5);
    quint8 clientid;
    quint16 id;
    quint16 dataCount;
    s >> clientid;
    if(m_clientId && (clientid != m_clientId)) {
        // Packet meant for another client, ignore
        return;
    }
    s >> dataCount;
    for(unsigned int i=0;i<dataCount;i++) {
        qint32 data;
        s >> id >> data;
        auto ref = m_idRefMap[id];
        if(id) {
            if(ref) {
                ref->updateValue(data);
            } else {
                qDebug() << Q_FUNC_INFO << "Can't find int ref value with id" << id;
            }
        }
    }
    s.skipRawData(2);
    dataCount = 0;
    s >> dataCount;
    for(unsigned int i=0;i<dataCount;i++) {
        float data;
        s >> id >> data;
        auto ref = m_idRefMap[id];
        if(id) {
            if(ref) {
                ref->updateValue(data);
            } else {
                qDebug() << Q_FUNC_INFO << "Can't find float ref value with id" << id;
            }
        }
    }
    s.skipRawData(2);
    dataCount = 0;
    s >> dataCount;
    for(unsigned int i=0;i<dataCount;i++) {
        double data;
        s >> id >> data;
        auto ref = m_idRefMap[id];
        if(id) {
            if(ref) {
                ref->updateValue(data);
            } else {
                qDebug() << Q_FUNC_INFO << "Can't find double ref value with id" << id;
            }
        }
    }
}
