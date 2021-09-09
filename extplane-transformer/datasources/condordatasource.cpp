#include "condordatasource.h"
#include <QNetworkDatagram>
#include <QDebug>
#include <QtMath>
#include <datarefs/dataref.h>

CondorDatasource::CondorDatasource() : DataSource()
  , m_port(55278)
  , m_updatesReceived(0)
{
    refMap.insert("sim/cockpit2/gauges/indicators/heading_electric_deg_mag_pilot", "compass");
    refMap.insert("sim/cockpit2/gauges/indicators/airspeed_kts_pilot", "airspeed");
    refMap.insert("sim/flightmodel/misc/h_ind", "altitude");
    refMap.insert("sim/flightmodel/position/vh_ind", "vario");
    refMap.insert("sim/cockpit2/gauges/indicators/slip_deg", "slipball");
    refMap.insert("sim/cockpit2/gauges/indicators/pitch_vacuum_deg_pilot", "pitch");
    refMap.insert("sim/cockpit2/gauges/indicators/roll_vacuum_deg_pilot", "bank");
    refMap.insert("sim/flightmodel2/misc/gforce_normal", "gforce");
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

        QStringList lines = data.split("\r\n");
        for(QString &line : lines) {
            QStringList splitLine = line.split("=");
            if(splitLine.length() == 2) {
                rxValue(splitLine[0], splitLine[1]);
            }
        }
        if(m_updatesReceived > 0) {
            setHelpText(QString("Received %1 value updates from Condor").arg(m_updatesReceived));
        }
    }
}

void CondorDatasource::rxValue(QString &param, QString &value) {
    bool ok = false;
    double val = value.toDouble(&ok);
    if(ok) {
        m_updatesReceived++;
    } else {
        qDebug() << Q_FUNC_INFO << "Failed to read value for " << param;
        return;
    }
    QString refName = refMap.key(param);
    if(refName.isEmpty()) return;

    // Magic happens here
    if(param == "pitch"
            || param == "bank"
            || param == "yaw"
            || param == "slipball")
        val = qRadiansToDegrees(val);
    if(param == "bank" || param == "slipball") val = -val;
    // Other units seem to be ok..
    //
    for(FloatDataRef *ref : floatRefs) {
        if(ref->name() == refName) {
            qDebug() << Q_FUNC_INFO << "Updating ref " << ref->name() << " to " << val;
            ref->updateValue(val);
        }
    }
}

DataRef *CondorDatasource::subscribeRef(QString &name) {
    if(refMap.contains(name)) {
        FloatDataRef *newRef = new FloatDataRef(this, name, nullptr);
        floatRefs.append(newRef);
        return newRef;
    }
    qDebug() << Q_FUNC_INFO << "Dataref " << name << "not supported!";
    return nullptr;
}

void CondorDatasource::unsubscribeRef(DataRef *ref) {
    if(ref->type() == extplaneRefTypeFloat)
        floatRefs.removeOne(qobject_cast<FloatDataRef*> (ref));
    ref->deleteLater();
}

void CondorDatasource::updateDataRef(DataRef *ref) {}

void CondorDatasource::changeDataRef(DataRef *ref) {}

void CondorDatasource::keyStroke(int keyid) {}

void CondorDatasource::buttonPress(int buttonid) {}

void CondorDatasource::buttonRelease(int buttonid) {}

void CondorDatasource::command(QString &name, extplaneCommandType type) {}

bool CondorDatasource::loadSituation(QString sitFileLocation) {
    return false;
}

void CondorDatasource::addFMSEntryLatLon(QString fmsEntryLine)
{}

void CondorDatasource::clearAllFmsEntries()
{}

void CondorDatasource::setDestinationFmsEntry(int index)
{}

