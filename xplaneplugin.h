#ifndef XPLANEPLUGIN_H
#define XPLANEPLUGIN_H

#include <QObject>
#include <QDebug>
#include <QStringList>
#include <QtCore/QCoreApplication>
#include "XPLMDataAccess.h"
#include "dataref.h"
#include "tcpserver.h"
#include "datarefprovider.h"

class XPlanePlugin : public QObject, public DataRefProvider {
    Q_OBJECT
public:
    explicit XPlanePlugin(QObject *parent = 0);
    ~XPlanePlugin();
    float flightLoop(float inElapsedSinceLastCall, float inElapsedTimeSinceLastFlightLoop,
                   int inCounter, void *inRefcon);
    int pluginStart(char * outName, char * outSig, char *outDesc);
    void pluginStop();

public: // DataRefProvider
    virtual DataRef *subscribeRef(QString name);
    virtual void unsubscribeRef(DataRef *ref);

signals:
   // void refSubscribed(DataRef *ref);
   // void refUnsubscribed(QString name);
   // void refChanged(DataRef *ref);
public slots:

private:
    QList<DataRef*> refs;
    int argc;
    char *argv;
    QCoreApplication *app;
    TcpServer *server;
};

#endif // XPLANEPLUGIN_H
