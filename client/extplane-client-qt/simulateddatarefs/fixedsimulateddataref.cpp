#include "fixedsimulateddataref.h"

FixedSimulatedDataRef::FixedSimulatedDataRef(QObject *parent, QString value, QString refName) :
    SimulatedDataRef(parent,0,0,1,false,0,refName), _value(value)
{
}

void FixedSimulatedDataRef::tickTime(double dt, int total) {
}

void FixedSimulatedDataRef::changeTimeout() {
    myClientRef.updateValue(_value);
}
