#ifndef EXTPLANECONNECTION_H
#define EXTPLANECONNECTION_H

#include <set>
#include <map>
#include <QTimer>
#include <QStringList>
#include "clientdatarefprovider.h"
#include "../../util/basictcpclient.h"

class SimulatedDataRef;
class ExtPlaneClient;
class ExtPlaneUdpClient;

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
    void extplaneWarning(QString message);

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
    std::set<ExtPlaneClient*> clients;
    std::map<QString, ClientDataRef*> dataRefs;
    bool server_ok = false;

private:
    double m_updateInterval = -1;
    unsigned int m_extplaneVersion = 0; // Version of the remote ExtPlane plugin. 0 if yet unknown.
    ExtPlaneUdpClient *m_udpClient = nullptr;
    quint8 m_clientId = 0;
};

#endif // EXTPLANECONNECTION_H

