#ifndef XPLANEPLUGIN_H
#define XPLANEPLUGIN_H

#include <QObject>
#include <QDebug>
#include <set>
#include <QStringList>
#include <QtCore/QCoreApplication>
#include "../extplane-server/tcpserver.h"
#include "../extplane-server/datarefprovider.h"
#include <XPLMDataAccess.h>
#include <XPLMMenus.h>

class DataRef;
class MQTTPublisher;

/**
  * The main X-Plane plugin class
  */
class XPlanePlugin : public QObject, public DataRefProvider {
    Q_OBJECT

public:
    explicit XPlanePlugin(QObject *parent = nullptr);

    ~XPlanePlugin();
    // X-Plane API
    float flightLoop(float inElapsedSinceLastCall, float inElapsedTimeSinceLastFlightLoop, int inCounter, void *inRefcon);
    int pluginStart(char * outName, char * outSig, char *outDesc);
    void pluginStop();
    int pluginEnable();
    void pluginDisable();

public: // DataRefProvider implementation
    virtual DataRef *subscribeRef(QString &name);
    virtual void unsubscribeRef(DataRef *ref);
    virtual void updateDataRef(DataRef *ref); // Update ref value from simulator
    virtual void keyStroke(int keyid);
    virtual void buttonPress(int buttonid);
    virtual void buttonRelease(int buttonid);
    virtual void changeDataRef(DataRef *ref);
    virtual void command(QString &name, extplaneCommandType type);
    virtual bool loadSituation(QString sitFileLocation);
    virtual void addFMSEntryLatLon(QString fmsEntryLine);
    virtual void clearAllFmsEntries();
    virtual void setDestinationFmsEntry(int index);

signals:
    void extplaneWarning(QString message); // Send warning message to client

public slots:
    void setFlightLoopInterval(float newInterval);

private:
    // Returns the ref name without modifier part (after :)
    QString refNameWithoutModifiers(QString &original);

    std::set<DataRef*> m_refs;
    int argc = 0; // Fake argc and argv for QCoreApplication
    char *argv = nullptr;
    QCoreApplication *m_app = nullptr; // For Qt main loop
    TcpServer *m_server = nullptr;
    MQTTPublisher *m_mqttPublisher = nullptr;
    float m_flightLoopInterval = -1; // Delay between loop calls (in seconds)
    int m_menu_container_idx = -1;
    XPLMMenuID m_menu_id = nullptr; // ExtPlane menus
};

#endif // XPLANEPLUGIN_H
