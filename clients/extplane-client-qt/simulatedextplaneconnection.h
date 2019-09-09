#ifndef SIMULATEDEXTPLANECONNECTION_H
#define SIMULATEDEXTPLANECONNECTION_H

#include <QTimer>
#include "extplaneconnection.h"

/**
 * @brief The SimulatedExtPlaneConnection class presents some
 * fake datarefs that can be used for testing if real
 * ExtPlane connection is not available.
 *
 * @see ExtPlaneConnection
 */
#define SIMULATED_TIMER_INTERVAL_MSEC 32

class SimulatedExtPlaneConnection : public ExtPlaneConnection {
    Q_OBJECT
public:
    explicit SimulatedExtPlaneConnection(QObject *parent = nullptr);
    virtual ~SimulatedExtPlaneConnection() {}

public slots:
    virtual void unsubscribeDataRef(ClientDataRef *ref);
    virtual void startConnection();
    virtual void stopConnection();
    virtual void keyPress(int id);
    virtual void buttonPress(int id);
    virtual void buttonRelease(int id);

private slots:
    void tickTime();
    virtual void connectionChangedSlot();

private:
    virtual void writeLine(QString line);
    virtual ClientDataRef *createDataRef(QString name, double accuracy=0);
    QList<SimulatedDataRef*> simulatedRefs;
    QTimer tickTimer;
};

#endif // SIMULATEDEXTPLANECONNECTION_H
