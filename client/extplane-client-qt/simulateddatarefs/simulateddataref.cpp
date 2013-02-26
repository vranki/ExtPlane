#include "simulateddataref.h"

SimulatedDataRef::SimulatedDataRef(QObject *parent, double minV, double maxV, double changeDivisor, bool round, QString refName) :
    QObject(parent), minValue(minV), maxValue(maxV), currentValue(0), change((maxValue - minValue)/changeDivisor), round(round), myClientRef(0, refName, 0)
{
    connect(&changeTimer, SIGNAL(timeout()), this, SLOT(changeTimeout()));
    changeTimer.setSingleShot(false);
    changeTimer.setInterval(5);
    changeTimer.start();
}

void SimulatedDataRef::tickTime(double dt, int total) {
    currentValue += change*dt;
    if(currentValue > maxValue)
        change = -qAbs(change);
    if(currentValue < minValue)
        change = qAbs(change);
}

void SimulatedDataRef::changeTimeout() {
    myClientRef.updateValue(QString::number(currentValue));
}

ClientDataRef *SimulatedDataRef::clientRef() {
    return &myClientRef;
}
