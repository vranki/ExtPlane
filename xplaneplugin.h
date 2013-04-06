#ifndef XPLANEPLUGIN_H
#define XPLANEPLUGIN_H

#include <QObject>
#include <QDebug>
#include <QStringList>
#include <QtCore/QCoreApplication>
#include "XPLMDataAccess.h"
#include "XPLMUtilities.h"
#include "tcpserver.h"
#include "datarefprovider.h"

class DataRef;

/**
  * The main plugin class
  */
class XPlanePlugin : public QObject, public DataRefProvider {
    Q_OBJECT
public:
    explicit XPlanePlugin(QObject *parent = 0);
    ~XPlanePlugin();
    float flightLoop(float inElapsedSinceLastCall, float inElapsedTimeSinceLastFlightLoop, int inCounter, void *inRefcon);
    int pluginStart(char * outName, char * outSig, char *outDesc);
    void pluginStop();
    void receiveMessage(XPLMPluginID inFromWho, long inMessage, void *  inParam);
public: // DataRefProvider
    virtual DataRef *subscribeRef(QString name);
    virtual void unsubscribeRef(DataRef *ref);
    virtual void keyStroke(int keyid);
    virtual void buttonPress(int buttonid);
    virtual void buttonRelease(int buttonid);
public slots:
    void setFlightLoopInterval(float newInterval);
private:
    QList<DataRef*> refs;
    int argc; // Fake argc and argv for QCoreApplication
    char *argv;
    QCoreApplication *app; // For Qt main loop
    TcpServer *server;
    float flightLoopInterval; // Delay between loop calls (in seconds)
};

#endif // XPLANEPLUGIN_H
