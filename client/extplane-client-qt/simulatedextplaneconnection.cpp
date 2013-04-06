#include "simulatedextplaneconnection.h"
#include "simulateddatarefs/simulateddataref.h"

SimulatedExtPlaneConnection::SimulatedExtPlaneConnection(QObject *parent) : ExtPlaneConnection(parent) {
    enableSimulatedRefs = true;
}

void SimulatedExtPlaneConnection::connectTo(QHostAddress addr, unsigned int port) {
    _addr = addr;
    _port = port;
    server_ok = true;
    emit connectionMessage("Connected to ExtPlane (simulated)");
}

ClientDataRef *SimulatedExtPlaneConnection::createDataRef(QString name, double accuracy) {
    SimulatedDataRef *simRef = 0;
    if(name=="sim/cockpit2/gauges/indicators/airspeed_kts_pilot") {
        simRef = new SimulatedDataRef(this, 0, 200, 2.0, false, name);
    } else if(name=="sim/flightmodel/position/vh_ind") {
        simRef = new SimulatedDataRef(this, -5, 5, 1.0, false, name);
    } else if(name=="sim/cockpit2/gauges/indicators/total_energy_fpm") {
        simRef = new SimulatedDataRef(this, -500, 500, 1.0, false, name);
    } else if(name=="sim/flightmodel/position/latitude") {
        simRef = new SimulatedDataRef(this, 47.3, 47.4, 1.0, false, name);
    } else if(name=="sim/flightmodel/position/longitude") {
        simRef = new SimulatedDataRef(this, 8.5, 8.6, 1.0, false, name);
    } else if(name=="sim/cockpit/misc/compass_indicated") {
        simRef = new SimulatedDataRef(this, 0, 360, 100.0, true, name);
    } else if(name=="sim/flightmodel/misc/h_ind") {
        simRef = new SimulatedDataRef(this, 0, 5000, 10.0, false, name);
    } else if(name=="sim/cockpit2/gauges/actuators/barometer_setting_in_hg_pilot") {
	simRef = new SimulatedDataRef(this, 0, 1, 100.0, false, name);
    } else if(name=="sim/cockpit2/gauges/indicators/heading_vacuum_deg_mag_pilot") {
	simRef = new SimulatedDataRef(this, 0, 360, 100.0, false, name);
    } else if(name=="sim/cockpit2/gauges/indicators/pitch_vacuum_deg_pilot") {
	simRef = new SimulatedDataRef(this, -90, 90, 5.0, false, name);
    } else if(name=="sim/cockpit2/gauges/indicators/roll_vacuum_deg_pilot") {
	simRef = new SimulatedDataRef(this, -90, 90, 10.0, false, name);
    } else if(name=="sim/cockpit2/gauges/indicators/roll_electric_deg_pilot") {
	simRef = new SimulatedDataRef(this, -30, 30, 10.0, false, name);
    } else if(name=="sim/cockpit2/gauges/indicators/slip_deg") {
	simRef = new SimulatedDataRef(this, -10, 10, 10.0, false, name);
    } else {
        // Fallback
        qWarning() << Q_FUNC_INFO << "the dataref " << name << "is not supported by simulation";
	simRef = new SimulatedDataRef(this, 0, 0, 1, false, name);
    }
    if(simRef) {
	simulatedRefs.append(simRef);
	return simRef->clientRef();
    }
    return 0;
}

void SimulatedExtPlaneConnection::unsubscribeDataRef(ClientDataRef *ref) {
    qDebug() << Q_FUNC_INFO << ref << ref->name() << ref->subscribers();
    ref->setSubscribers(ref->subscribers() - 1);
    if(ref->subscribers() > 0) return;
    qDebug() << Q_FUNC_INFO << "Ref not subscribed by anyone anymore";
    dataRefs.remove(ref->name());

    foreach(SimulatedDataRef *simRef, simulatedRefs) {
	if(simRef->clientRef()==ref) {
	    simRef->deleteLater();
            simulatedRefs.removeOne(simRef);
	}
    }
    disconnect(ref, 0, this, 0);

    foreach(ClientDataRef *ref, dataRefs) {
	qDebug() << "refs now:" << ref->name();
    }
}




void SimulatedExtPlaneConnection::writeLine(QString line) {
    qDebug() << Q_FUNC_INFO << line << "(simulated)";
}



void SimulatedExtPlaneConnection::tickTime(double dt, int total) {
    foreach(SimulatedDataRef *dr, simulatedRefs)
	dr->tickTime(dt, total);
}
