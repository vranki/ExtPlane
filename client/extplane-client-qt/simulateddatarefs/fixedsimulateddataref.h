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
    FixedSimulatedDataRef(QObject *parent, QString value, QString refName);
public slots:
    virtual void tickTime(double dt, int total);
private slots:
    virtual void changeTimeout();
protected:
    QString _value;
};




class NavDataFixedSimulatedDataRef : public FixedSimulatedDataRef
{
    Q_OBJECT
public:
    /**
     * @brief A Simulated DataRef which returns a random set of NavData. The aim of this
     * class is to provide a quick way to test navdata drawing - the data it returns
     * does not necessarily make sense.
     * @param parent: the parent connection (should be a SimulatedExtPlanceConnection)
     * @param range: the range for the navdata (in meters)
     * @param refName: the name of the DataRef (such as sim/aircraft/view/acf_tailnum)
     */
    NavDataFixedSimulatedDataRef(QObject *parent, float range, QString refName);
};

#endif // FIXEDSIMULATEDDATAREF_H
