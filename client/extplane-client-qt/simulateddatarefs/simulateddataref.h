#ifndef SIMULATEDDATAREF_H
#define SIMULATEDDATAREF_H

#include <QObject>
#include <QTimer>
#include "clientdataref.h"

/**
 * Generic simulated dataref that can be configured to alter value.
 *
 * @see SimulatedExtPlaneConnection
 */
class SimulatedDataRef : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief DataRef that simulates through a range specified in the constructor. The DataRef will return values between minV and maxV in increments of (maxValue-minValue)/changeDivisor. Thus, the larger the changeDivisor the finer the DataRef will iterate through the values.
     * @param parent: the parent connection (should be a SimulatedExtPlanceConnection)
     * @param minV: the minimum value
     * @param maxV: the maximum value
     * @param changeDivisor: slows down the progression from minV to maxV (when 1, dx is 1)
     * @param round: if true, the value is rounded to a whole number
     * @param arrayCount: If > 0 specifies the number of datarefs for array datarefs in the format "[x,y,z]". If < 0 specifies number of dararefs for array datarefs in the format "x y z". If 0 specifies a single dataref (non-array).
     * @param refName: the name of the DataRef (such as sim/flightmodel/position/vh_ind)
     */
    explicit SimulatedDataRef(QObject *parent, double minV, double maxV, double changeDivisor, bool round, int arrayCount, QString refName);
    virtual ~SimulatedDataRef();
    ClientDataRef *clientRef();

signals:
    
public slots:
    virtual void tickTime(double dt, int total);
private slots:
    virtual void changeTimeout();
protected:
    double minValue, maxValue, change, currentValue;
    bool round;
    int arrayCount;
    QTimer changeTimer;
    ClientDataRef myClientRef;
};

#endif // SIMULATEDDATAREF_H
