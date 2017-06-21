#include "alternatingsimulateddataref.h"

#include "../../util/console.h"

AlternatingSimulatedDataRef::AlternatingSimulatedDataRef(QObject *parent, QString values, double secondsPerValue, QString refName) :
    SimulatedDataRef(parent,0,0,1,false,0,refName)
{
    _values = values.split(",");
    _currentIndex = 0;
    _currentValue = _values.at(0);
    _time = 0;
    _secondsPerValue = secondsPerValue;
}

void AlternatingSimulatedDataRef::tickTime(double dt, int total) {
    Q_UNUSED(total);
    _time += dt;
    if(_time > _secondsPerValue) {
        _time = 0;
        _currentIndex = (_currentIndex+1)%_values.count();
        _currentValue = _values.at(_currentIndex);
        INFO << "new value" << _currentValue;
    }
}

void AlternatingSimulatedDataRef::changeTimeout() {
    myClientRef.updateValue(_currentValue);
}
