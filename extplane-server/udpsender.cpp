#include "udpsender.h"
#include "datarefs/dataref.h"
#include "datarefs/floatdataref.h"
#include "datarefs/intdataref.h"
#include "datarefs/doubledataref.h"
#include "datarefprovider.h"
#include "console.h"
#include "../protocoldefs.h"
#include <QNetworkDatagram>

UdpSender::UdpSender(DataRefProvider *refProvider, QHostAddress host, quint8 clientId, QObject *parent) : QObject(parent)
  , m_refProvider(refProvider)
  , m_host(host)
  , m_clientId(clientId)
{
    Q_ASSERT(m_refProvider);
    m_udpPort = UDP_OUT_PORT_BASE + m_clientId;
    INFO << "Created ExtPlane UdpSender for peer" << m_host << "client id" << QString::number(m_clientId) << "sending to port" << m_udpPort;
}

bool UdpSender::subscribedRef(DataRef *ref)
{
    Q_ASSERT(ref->udpId());
    connect(ref, &DataRef::changed, this, &UdpSender::refChanged);
    return true;
}

void UdpSender::unsubscribedRef(DataRef *ref)
{
    disconnect(ref, 0, this, 0);
}

void UdpSender::flightLoop()
{
    if(!m_countInt && !m_countFloat && !m_countDouble) return;

    QByteArray buf;
    QDataStream s(&buf, QIODevice::WriteOnly);

    s.writeRawData("EXTP_", 5);
    s << (quint8 ) m_clientId;
    s << (quint16) m_countInt;
    s.writeRawData(m_bufInt.data(), m_bufInt.size());

    s.writeRawData("Ef", 2);
    s << (quint16) m_countFloat;
    s.writeRawData(m_bufFloat.data(), m_bufFloat.size());

    s.writeRawData("Ed", 2);
    s << (quint16) m_countDouble;
    s.writeRawData(m_bufDouble.data(), m_bufDouble.size());

    auto outSize = m_socket.writeDatagram(buf, m_host, m_udpPort);
    if(outSize != buf.size()) {
        INFO << "Failed sending UDP datagram! Sent only" << outSize << "bytes of" << buf.size();
    } else {
        // INFO << "Sent" << outSize << "bytes to" << m_host << m_udpPort << "client id" << m_clientId << "counts" << m_countInt << m_countFloat << m_countDouble;
    }

    m_bufInt.clear();
    // m_bufInt.reserve(BUFFER_RESERVE);
    m_countInt = 0;

    m_bufFloat.clear();
    // m_bufFloat.reserve(BUFFER_RESERVE);
    m_countFloat = 0;

    m_bufDouble.clear();
    // m_bufDouble.reserve(BUFFER_RESERVE);
    m_countDouble = 0;

    Q_ASSERT(m_bufInt.isEmpty());
    Q_ASSERT(m_bufFloat.isEmpty());
    Q_ASSERT(m_bufDouble.isEmpty());
}

void UdpSender::refChanged(DataRef *ref) {
    Q_ASSERT(ref);
    Q_ASSERT(ref->udpId());
    if(ref->type() == extplaneRefTypeInt) {
        auto intRef = qobject_cast<IntDataRef*>(ref);
        QDataStream s(&m_bufInt, QIODevice::WriteOnly);
        s << (quint16) intRef->udpId() << (qint32) intRef->value();
        m_countInt++;
    } else if(ref->type() == extplaneRefTypeFloat) {
        auto floatRef = qobject_cast<FloatDataRef*>(ref);
        QDataStream s(&m_bufFloat, QIODevice::WriteOnly);
        s << (quint16) floatRef->udpId() << (float) floatRef->value(); // Float is 4 bytes always
        m_countFloat++;
    } else if(ref->type() == extplaneRefTypeDouble) {
        auto doubleRef = qobject_cast<DoubleDataRef*>(ref);
        QDataStream s(&m_bufDouble, QIODevice::WriteOnly);
        s << (quint16) doubleRef->udpId() << (double) doubleRef->value(); // Double is 8 bytes always
        m_countDouble++;
    }
}
