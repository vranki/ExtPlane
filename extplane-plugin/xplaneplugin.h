#ifndef XPLANEPLUGIN_H
#define XPLANEPLUGIN_H

#include <QObject>
#include <QDebug>
#include <QStringList>
#include <QtCore/QCoreApplication>
#include "../extplane-server/tcpserver.h"
#include "../extplane-server/datarefprovider.h"
#include <XPLMDataAccess.h>

class DataRef;

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
    void receiveMessage(XPLMPluginID inFromWho, long inMessage, void *  inParam);

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

public slots:
    void setFlightLoopInterval(float newInterval);

private:
    // Returns the ref name without modifier part (after :)
    QString refNameWithoutModifiers(QString &original);

    QList<DataRef*> refs;
    int argc; // Fake argc and argv for QCoreApplication
    char *argv;
    QCoreApplication *app; // For Qt main loop
    TcpServer *server;
    float flightLoopInterval; // Delay between loop calls (in seconds)
};

#endif // XPLANEPLUGIN_H
