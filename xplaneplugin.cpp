#include "xplaneplugin.h"

XPlanePlugin::XPlanePlugin(QObject *parent) :
    QObject(parent), argc(0), argv(0), app(0), server(0) {
}

XPlanePlugin::~XPlanePlugin() {
}

float XPlanePlugin::flightLoop(float inElapsedSinceLastCall, float inElapsedTimeSinceLastFlightLoop,
                               int inCounter, void *inRefcon) {
    foreach(DataRef *ref, refs) {
        ref->setValue(XPLMGetDataf(ref->ref()));
    }
    app->processEvents();
    return 0.016;
}

int XPlanePlugin::pluginStart(char * outName, char * outSig, char *outDesc) {
    qDebug() << Q_FUNC_INFO;
    app = new QCoreApplication(argc, &argv);
    server = new TcpServer(this, this);
    strcpy(outName, "ExtPlane");
    strcpy(outSig, "org.vranki.extplaneplugin");
    strcpy(outDesc, "Read and write X-Plane datarefs from external programs using TCP socket.");
    app->processEvents();
    return 1;
}

DataRef* XPlanePlugin::subscribeRef(QString name) {
    qDebug() << Q_FUNC_INFO << name;
    foreach(DataRef *ref, refs) {
        if(ref->name()==name) {
            qDebug() << Q_FUNC_INFO << "Already subscribed to " << name;
            ref->setSubscribers(ref->subscribers() + 1);
            return ref;
        }
    }

    XPLMDataRef ref = XPLMFindDataRef(name.toAscii());
    if(ref) {
        DataRef *dr = new DataRef(this, name, ref);
        dr->setSubscribers(1);
        qDebug() << Q_FUNC_INFO << "Subscribed to ref " << dr->name();
        refs.append(dr);
        return dr;
    } else {
        qDebug() << Q_FUNC_INFO << "Can't find dataref " << name;
        return 0;
    }
}

void XPlanePlugin::unsubscribeRef(DataRef *ref) {
    Q_ASSERT(refs.contains(ref));
    qDebug() << Q_FUNC_INFO << ref->name() << ref->subscribers();
    ref->setSubscribers(ref->subscribers() - 1);
    if(ref->subscribers() == 0) {
        refs.removeOne(ref);
        qDebug() << Q_FUNC_INFO << "Ref " << ref->name() << " not subscribed by anyone - removing.";
        ref->deleteLater();
    }
}

void XPlanePlugin::pluginStop() {
    qDebug() << Q_FUNC_INFO;
    qDeleteAll(refs);
    refs.clear();
    app->processEvents();
    delete server;
    server = 0;
    app->quit();
    app->processEvents();
    delete app;
    app = 0;
}
