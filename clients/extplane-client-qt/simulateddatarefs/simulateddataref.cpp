#include "simulateddataref.h"
#include <QDebug>
#include <qmath.h>

SimulatedDataRef::SimulatedDataRef(QObject *parent,
                                   double minV,
                                   double maxV,
                                   double changeDivisor,
                                   bool round,
                                   int arrayCount,
                                   QString refName) :
    QObject(parent)
  , minValue(minV)
  , maxValue(maxV)
  , change((maxValue - minValue)/changeDivisor)
  , currentValue(minV)
  , actualCurrentValue(minV)
  , round(round)
  , arrayCount(arrayCount)
  , myClientRef(nullptr, refName, 0)
{
    connect(&changeTimer, &QTimer::timeout, this, &SimulatedDataRef::changeTimeout);
    changeTimer.setSingleShot(false);
    changeTimer.setInterval(16);
    changeTimer.start();
}

SimulatedDataRef::~SimulatedDataRef() { }

void SimulatedDataRef::tickTime(double dt) {
    actualCurrentValue += change*dt;
    if(actualCurrentValue > maxValue)
        change = -qAbs(change);
    if(actualCurrentValue < minValue)
        change = qAbs(change);
    if(round) currentValue = qRound(actualCurrentValue);
    else currentValue = actualCurrentValue;
}

void SimulatedDataRef::changeTimeout() {
    if(arrayCount > 0) {
        QStringList values;
        for(int i=0;i<arrayCount;i++){
            // For every additional array value we use the current value divided by its index,
            // this easily simulates different array values for a dataref that are constrained to
            // the defined range.
            values.push_back(QString("%1").arg(currentValue/static_cast<double>(i+1)));
        }
        myClientRef.updateValue(values);
    }else {
        myClientRef.updateValue(QString::number(currentValue));
    }
}

ClientDataRef *SimulatedDataRef::clientRef() {
    return &myClientRef;
}
