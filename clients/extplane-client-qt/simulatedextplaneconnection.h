#ifndef SIMULATEDEXTPLANECONNECTION_H
#define SIMULATEDEXTPLANECONNECTION_H

#include "extplaneconnection.h"

/**
 * @brief The SimulatedExtPlaneConnection class presents some
 * fake datarefs that can be used for testing if real
 * ExtPlane connection is not available.
 *
 * @see ExtPlaneConnection
 */
class SimulatedExtPlaneConnection : public ExtPlaneConnection {
    Q_OBJECT
public:
    explicit SimulatedExtPlaneConnection(QObject *parent = 0);
    virtual ~SimulatedExtPlaneConnection() {}
public slots:
    virtual void unsubscribeDataRef(ClientDataRef *ref);
    virtual void connectTo(QString host, unsigned int port);
    void tickTime(double dt, int total);
private:
    virtual void writeLine(QString line);
    virtual ClientDataRef *createDataRef(QString name, double accuracy=0);
    QList<SimulatedDataRef*> simulatedRefs;
};

#endif // SIMULATEDEXTPLANECONNECTION_H
