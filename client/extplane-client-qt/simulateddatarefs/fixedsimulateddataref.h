#ifndef FIXEDSIMULATEDDATAREF_H
#define FIXEDSIMULATEDDATAREF_H

#include "simulateddataref.h"

class FixedSimulatedDataRef : public SimulatedDataRef
{
    Q_OBJECT
public:
    /**
     * @brief A Simulated DataRef which is fixed to a single value.
     * @param parent: the parent connection (should be a SimulatedExtPlanceConnection)
     * @param value: the fixed value in string format
     * @param refName: the name of the DataRef (such as sim/aircraft/view/acf_tailnum)
     */
    explicit FixedSimulatedDataRef(QObject *parent, QString value, QString refName);
public slots:
    virtual void tickTime(double dt, int total);
private slots:
    virtual void changeTimeout();
private:
    QString _value;
};

#endif // FIXEDSIMULATEDDATAREF_H
