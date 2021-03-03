#include "udpsender.h"
#include "datarefs/dataref.h"
#include "datarefs/floatdataref.h"
#include "datarefs/intdataref.h"
#include "datarefs/doubledataref.h"
#include "datarefprovider.h"
#include "console.h"
#include "../protocoldefs.h"
#include <QNetworkDatagram>
#include <QDataStream>

UdpSender::UdpSender(DataRefProvider *refProvider, QHostAddress host, quint8 clientId, QObject *parent) : QObject(parent)
  , m_refProvider(refProvider)
  , m_host(host)
  , m_clientId(clientId)
{
    Q_ASSERT(m_refProvider);
    Q_ASSERT(m_clientId);
    m_udpPort = UDP_OUT_PORT_BASE + m_clientId;
    INFO << "Created ExtPlane UdpSender for peer" << m_host << "client id" << QString::number(m_clientId) << "sending to port" << QString::number(m_udpPort);
}

bool UdpSender::subscribedRef(DataRef *ref)
{
    Q_ASSERT(ref->udpId());
    connect(ref, &DataRef::changed, this, &UdpSender::refChanged);
    return true;
}

void UdpSender::unsubscribedRef(DataRef *ref)
{
    m_changedRefs.erase(ref);
    disconnect(ref, 0, this, 0);
}

void UdpSender::flightLoop()
{
    Q_ASSERT(m_clientId);

    QByteArray buf;
    QDataStream s(&buf, QIODevice::WriteOnly);

    s.writeRawData("EXTP_", 5);
    s << m_clientId;

    s << countRefs(extplaneRefTypeInt);
    for(auto ref : m_changedRefs) {
        if(ref->type() == extplaneRefTypeInt) {
            s << ref->udpId() << qobject_cast<IntDataRef*>(ref)->value();
        }
    }
    s.writeRawData("Ef", 2);
    s << countRefs(extplaneRefTypeFloat);
    s.setFloatingPointPrecision(QDataStream::SinglePrecision);
    for(auto ref : m_changedRefs) {
        if(ref->type() == extplaneRefTypeFloat) {
            s << ref->udpId() << qobject_cast<FloatDataRef*>(ref)->value();
        }
    }

    s.writeRawData("Ed", 2);
    s << countRefs(extplaneRefTypeDouble);
    s.setFloatingPointPrecision(QDataStream::DoublePrecision);
    for(auto ref : m_changedRefs) {
        if(ref->type() == extplaneRefTypeDouble) {
            s << ref->udpId() << qobject_cast<DoubleDataRef*>(ref)->value();
        }
    }
    auto outSize = m_socket.writeDatagram(buf, m_host, m_udpPort);
    if(outSize != buf.size()) {
        INFO << "Failed sending UDP datagram! Sent only" << outSize << "bytes of" << buf.size();
    } else {
        // INFO << "Sent" << outSize << "bytes to" << m_host << m_udpPort << "client id" << m_clientId << "counts" << m_countInt << m_countFloat << m_countDouble;
    }
    m_changedRefs.clear();
}

void UdpSender::refChanged(DataRef *ref) {
    Q_ASSERT(ref);
    Q_ASSERT(ref->udpId());
    m_changedRefs.insert(ref);
}

quint16 UdpSender::countRefs(int type) {
    quint16 res = 0;
    for(auto ref : m_changedRefs) {
        if(ref->type() == type) res++;
    }
    return res;
}
