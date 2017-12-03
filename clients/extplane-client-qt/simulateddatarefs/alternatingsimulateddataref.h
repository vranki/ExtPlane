#ifndef ALTERNATINGSIMULATEDDATAREF_H
#define ALTERNATINGSIMULATEDDATAREF_H

#include "simulateddataref.h"

class AlternatingSimulatedDataRef : public SimulatedDataRef
{
    Q_OBJECT
public:
    /**
     * @brief A Simulated DataRef which alternates a series of fixed values.
     * @param parent: the parent connection (should be a SimulatedExtPlanceConnection)
     * @param values: the fixed value in comma seperated format
     * @param secondsPerValue: how long each value is rotated for
     * @param refName: the name of the DataRef (such as sim/aircraft/view/acf_tailnum)
     */
    AlternatingSimulatedDataRef(QObject *parent, QString values, double secondsPerValue, QString refName);
public slots:
    virtual void tickTime(double dt, int total);
private slots:
    virtual void changeTimeout();
protected:
    QStringList _values;
    QString _currentValue;
    int _currentIndex;
    double _time;
    double _secondsPerValue;
};

#endif // ALTERNATINGSIMULATEDDATAREF_H
