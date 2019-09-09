#ifndef EXTPLANECLIENT_H
#define EXTPLANECLIENT_H

#include <QObject>
#include <QString>
#include <QList>
#include <QSet>
#include "clientdataref.h"
#include "extplaneconnection.h"
#include "clientdatarefprovider.h"
#include "simulatedextplaneconnection.h"

/**
 * Represents a single ExtPlane client (client side)
 * Can be simulated or real connection.
 *
 * Call createClient() after creating object.
 */
class ExtPlaneClient : public QObject {
    Q_OBJECT
    Q_PROPERTY(ClientDataRefProvider *datarefProvider READ datarefProvider NOTIFY datarefProviderChanged)
    Q_PROPERTY(ExtPlaneConnection *extplaneConnection READ extplaneConnection CONSTANT)
    Q_PROPERTY(bool simulated READ isSimulated WRITE setSimulated NOTIFY simulatedChanged)
    Q_PROPERTY(QString connectionMessage READ connectionMessage NOTIFY connectionMessageChanged)

public:
    ExtPlaneClient(ExtPlaneClient const&)  = delete;
    void operator=(ExtPlaneClient const&)  = delete;
    explicit ExtPlaneClient(bool simulated = false);
    explicit ExtPlaneClient(QObject *parent, QString name, bool simulated);
    ~ExtPlaneClient();
    static ExtPlaneClient &instance();
    Q_INVOKABLE void createClient(); // MUST be called before use
    ClientDataRef* subscribeDataRef(QString name, double accuracy=0);
    void unsubscribeDataRef(QString name);
    bool isDataRefSubscribed(QString name);
    void keyPress(int id);
    void buttonPress(int id);
    void buttonRelease(int id);
    void commandOnce(QString name);
    void commandBegin(QString name);
    void commandEnd(QString name);
    bool isSimulated() const;
    ClientDataRefProvider* datarefProvider() const;
    ExtPlaneConnection *extplaneConnection();
    QString connectionMessage();

public slots:
    void setUpdateInterval(double newInterval);
    void setSimulated(bool simulated);

signals:
    void refChanged(QString name, double value);
    void refChanged(QString name, QString value);
    void refChanged(QString name, QStringList values);
    void datarefProviderChanged(ClientDataRefProvider* datarefProvider);
    void simulatedChanged(bool simulated);
    void connectionMessageChanged(QString connectionMessage);

private slots:
    void cdrChanged(ClientDataRef *ref);
    void valueSet(ClientDataRef *ref);
    void unsubscribed(ClientDataRef *ref);
    void refDestroyed(QObject* refqo);
    void setConnectionMessage(QString msg);

private:
    QString m_name;
    QList<ClientDataRef*> m_dataRefs;
    QSet<int> m_heldButtons;
    QSet<QString> m_runningCommands;
    ClientDataRefProvider *m_connection; // The actual connection class
    bool m_simulated;
    ExtPlaneConnection m_extplaneConnection;
    SimulatedExtPlaneConnection m_simulatedExtplaneConnection;
    QString m_connectionMessage;
    static ExtPlaneClient* _instance;
};

#endif // EXTPLANECLIENT_H
