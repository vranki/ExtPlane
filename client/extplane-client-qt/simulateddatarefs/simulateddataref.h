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
     * @todo Document these
     *
     * @brief SimulatedDataRef
     * @param parent
     * @param minV
     * @param maxV
     * @param changeDivisor
     * @param round
     * @param refName
     */
    explicit SimulatedDataRef(QObject *parent, double minV, double maxV, double changeDivisor, bool round, QString refName);
    ClientDataRef *clientRef();
signals:
    
public slots:
    virtual void tickTime(double dt, int total);
private slots:
    void changeTimeout();
private:
    double minValue, maxValue, change, currentValue;
    bool round;
    QTimer changeTimer;
    ClientDataRef myClientRef;
};

#endif // SIMULATEDDATAREF_H
