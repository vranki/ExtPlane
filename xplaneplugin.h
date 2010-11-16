#ifndef XPLANEPLUGIN_H
#define XPLANEPLUGIN_H

#include <QObject>
#include <QDebug>
#include <QStringList>
#include <QtCore/QCoreApplication>
#include "XPLMDataAccess.h"
#include "floatdataref.h"
#include "intdataref.h"
#include "doubledataref.h"
#include "tcpserver.h"
#include "datarefprovider.h"
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

public: // DataRefProvider
    virtual DataRef *subscribeRef(QString name);
    virtual void unsubscribeRef(DataRef *ref);
private:
    QList<DataRef*> refs;
    int argc; // Fake argc and argv for QCoreApplication
    char *argv;
    QCoreApplication *app; // For Qt main loop
    TcpServer *server;
};

#endif // XPLANEPLUGIN_H
