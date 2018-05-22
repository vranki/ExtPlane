#include "condordatasource.h"
#include <QNetworkDatagram>
#include <QDebug>
#include <datarefs/dataref.h>

CondorDatasource::CondorDatasource() : DataSource() {
    m_port = 55278;

    m_supportedRefs << "sim/cockpit2/gauges/indicators/heading_electric_deg_mag_pilot"
                    << "sim/cockpit2/gauges/indicators/airspeed_kts_pilot"
                    << "sim/cockpit2/gauges/indicators/pitch_vacuum_deg_pilot"
                    << "sim/cockpit2/gauges/indicators/roll_vacuum_deg_pilot"
                    << "sim/flightmodel/misc/h_ind"
                    << "sim/flightmodel/position/vh_ind"
                    << "sim/cockpit2/gauges/indicators/slip_deg";
}


void CondorDatasource::connectToSource() {
    setNetworkError(QString());

    connect(&m_udpSocket, &QUdpSocket::readyRead, this, &CondorDatasource::readPendingDatagrams);
    if(m_udpSocket.bind(QHostAddress::LocalHost, m_port)) {
        setHelpText(QString("Listening to UDP transmissions on port %1. Please run Condor.\nSee config help at http://www.condorsoaring.com/manual/#simkits-and-udp-outputs").arg(m_port));
    } else {
        setNetworkError(QString("Unable to bind to UDP port %1!").arg(m_port));
    }
}

void CondorDatasource::readPendingDatagrams() {
    while (m_udpSocket.hasPendingDatagrams()) {
        QNetworkDatagram datagram = m_udpSocket.receiveDatagram();
        QString data = QString::fromUtf8(datagram.data());
        qDebug() << Q_FUNC_INFO << "RX datagram" << data;

        QStringList lines = data.split("\n");
        for(QString line : lines) {
            QStringList splitLine = line.split("=");
            if(splitLine.length() == 2) {
                QString param = splitLine[0];
                QString value = splitLine[1];
                rxValue(param, value);
            }
        }
    }
}

void CondorDatasource::rxValue(QString &param, QString &value) {
    qDebug() << Q_FUNC_INFO << param << value;
}

DataRef *CondorDatasource::subscribeRef(QString &name)
{
    if(m_supportedRefs.contains(name)) {
        FloatDataRef *newRef = new FloatDataRef(this, name, nullptr);
        floatRefs.append(newRef);
        return newRef;
    }
    return nullptr;
}

void CondorDatasource::unsubscribeRef(DataRef *ref)
{
    qDebug() << Q_FUNC_INFO << ref->name();
    if(ref->type() == extplaneRefTypeFloat)
        floatRefs.removeOne(qobject_cast<FloatDataRef*> (ref));
    ref->deleteLater();
}

void CondorDatasource::updateDataRef(DataRef *ref)
{}

void CondorDatasource::changeDataRef(DataRef *ref)
{}

void CondorDatasource::keyStroke(int keyid)
{}

void CondorDatasource::buttonPress(int buttonid)
{}

void CondorDatasource::buttonRelease(int buttonid)
{}

void CondorDatasource::command(QString &name, extplaneCommandType type)
{}

bool CondorDatasource::loadSituation(QString sitFileLocation)
{}
