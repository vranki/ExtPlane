#ifndef EXTPLANECONNECTION_H
#define EXTPLANECONNECTION_H

#include <QList>
#include <QMap>
#include <QTimer>
#include <QStringList>
#include "clientdatarefprovider.h"
#include "../../util/basictcpclient.h"

class SimulatedDataRef;
class ExtPlaneClient;

/**
 * A TCP-based connector class which connects to ExtPlane
 * running in X-Plane.
 */
class ExtPlaneConnection : public BasicTcpClient, public ClientDataRefProvider {
    Q_OBJECT

public:
    explicit ExtPlaneConnection(QObject *parent = 0);
    virtual ~ExtPlaneConnection() {}
    void registerClient(ExtPlaneClient* client);

public slots:
    virtual ClientDataRef *subscribeDataRef(QString name, double accuracy=0);
    virtual void unsubscribeDataRef(ClientDataRef *ref);
    virtual void keyPress(int id);
    virtual void buttonPress(int id);
    virtual void buttonRelease(int id);
    virtual void commandOnce(QString name);
    virtual void commandBegin(QString name);
    virtual void commandEnd(QString name);
    virtual void setValue(QString name, QString value);
    virtual void setValues(QString name, QStringList values);
    virtual void setValue(ClientDataRef *ref);
    void setUpdateInterval(double newInterval);
    void tickTime(double dt, int total);
    void receivedLineSlot(QString &line);

private slots:
    void tcpClientConnected();
    void socketError(QAbstractSocket::SocketError err);

protected:
    void subRef(ClientDataRef *ref);
    void writeLine(QString line);
    virtual ClientDataRef *createDataRef(QString newName, double accuracy=0);
    QList<ExtPlaneClient*> clients;
    QMap<QString, ClientDataRef*> dataRefs;
    bool server_ok;
    QList<SimulatedDataRef*> simulatedRefs;
    bool enableSimulatedRefs;
private:
    double updateInterval;
};

#endif // EXTPLANECONNECTION_H

