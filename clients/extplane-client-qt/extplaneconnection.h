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
    Q_INTERFACES(ClientDataRefProvider)

public:
    explicit ExtPlaneConnection(QObject *parent = nullptr);
    virtual ~ExtPlaneConnection() {}
    void registerClient(ExtPlaneClient* client);

signals:
    void connectionMessage(QString connectionMessage);

public slots:
    virtual ClientDataRef *subscribeDataRef(QString name, double accuracy = 0);
    virtual void unsubscribeDataRef(ClientDataRef *ref);
    virtual void keyPress(int id);
    virtual void buttonPress(int id);
    virtual void buttonRelease(int id);
    virtual void commandOnce(QString name);
    virtual void commandBegin(QString name);
    virtual void commandEnd(QString name);
    virtual void setValue(QString name, QString value);
    virtual void setValues(QString name, QStringList values);
    virtual void setValueFromRef(ClientDataRef *ref); // Convenience, calls above function with ref's values.
    void setUpdateInterval(double newInterval);
    void receivedLineSlot(QString &line);
    virtual void startConnection();
    virtual void stopConnection();

private slots:
    void connectionChangedSlot();
    void connectedChanged(bool connected);
    void socketError(QAbstractSocket::SocketError err);

protected:
    void subRef(ClientDataRef *ref);
    virtual void writeLine(QString line);
    virtual ClientDataRef *createDataRef(QString newName, double accuracy = 0);
    QList<ExtPlaneClient*> clients;
    QMap<QString, ClientDataRef*> dataRefs;
    bool server_ok;

private:
    double m_updateInterval;
    int m_extplaneVersion; // Version of the remote ExtPlane plugin. -1 if yet unknown.
};

#endif // EXTPLANECONNECTION_H

