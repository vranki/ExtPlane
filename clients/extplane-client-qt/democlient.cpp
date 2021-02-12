#include "democlient.h"
#include <QCoreApplication>
#include "extplaneconnection.h"

DemoClient::DemoClient(QObject *parent) : QObject(parent) {}

void DemoClient::run()
{
    m_client.createClient();
    connect(m_client.extplaneConnection(), &QTcpSocket::connected, this, &DemoClient::connected);
    m_client.extplaneConnection()->setHostName("127.0.0.1");
}

void DemoClient::connected()
{
    qDebug() << Q_FUNC_INFO;
    m_secondsRef = m_client.subscribeDataRef("sim/cockpit2/clock_timer/zulu_time_seconds:udp");
    connect(m_secondsRef, &ClientDataRef::changed, this, &DemoClient::refChanged, Qt::UniqueConnection);
    //m_elevationRef = m_client.subscribeDataRef("sim/flightmodel/position/elevation:udp");
    //connect(m_elevationRef, &ClientDataRef::changed, this, &DemoClient::refChanged);
    //m_viewHeadingRef = m_client.subscribeDataRef("sim/graphics/view/view_heading:udp");
    //connect(m_viewHeadingRef, &ClientDataRef::changed, this, &DemoClient::refChanged);
}

void DemoClient::refChanged(ClientDataRef *ref)
{
    qDebug() << Q_FUNC_INFO << ref->name() << ref->value() << ref->udpId();
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    DemoClient dc;
    dc.run();
    return app.exec();
}
