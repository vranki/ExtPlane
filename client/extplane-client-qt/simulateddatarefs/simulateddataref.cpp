#include "simulateddataref.h"

SimulatedDataRef::SimulatedDataRef(QObject *parent, double minV, double maxV, double changeDivisor, bool round, int arrayCount, QString refName) :
    QObject(parent), minValue(minV), maxValue(maxV), currentValue(minV), change((maxValue - minValue)/changeDivisor), round(round), arrayCount(arrayCount), myClientRef(0, refName, 0)
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
    if(arrayCount > 0) {
        QString ret="[";
        for(int i=0;i<arrayCount;i++){
            ret += QString::number(currentValue) + (i < arrayCount -1 ? QString(",") : "");
        }
        ret += "]";
        myClientRef.updateValue(ret);
    } else if(arrayCount < 0) {
        QString ret="";
        for(int i=0;i<qAbs(arrayCount);i++){
            ret += QString::number(currentValue) + (i < qAbs(arrayCount) -1 ? QString(" ") : "");
        }
        myClientRef.updateValue(ret);
    } else {
        myClientRef.updateValue(QString::number(currentValue));
    }
}

ClientDataRef *SimulatedDataRef::clientRef() {
    return &myClientRef;
}
