#include "xplaneplugin.h"
#include "datarefs/dataref.h"
#include "datarefs/floatdataref.h"
#include "datarefs/floatarraydataref.h"
#include "datarefs/intdataref.h"
#include "datarefs/intarraydataref.h"
#include "datarefs/doubledataref.h"
#include "datarefs/datadataref.h"

XPlanePlugin::XPlanePlugin(QObject *parent) :
    QObject(parent), argc(0), argv(0), app(0), server(0), flightLoopInterval(0.31f) { // Default to 30hz
}

XPlanePlugin::~XPlanePlugin() {
    qDebug() << Q_FUNC_INFO;
}

float XPlanePlugin::flightLoop(float inElapsedSinceLastCall, float inElapsedTimeSinceLastFlightLoop,
                               int inCounter, void *inRefcon) {
    foreach(DataRef *ref, refs)
        ref->updateValue();
    app->processEvents();
    return flightLoopInterval;
}

int XPlanePlugin::pluginStart(char * outName, char * outSig, char *outDesc) {
    qDebug() << Q_FUNC_INFO << "ExtPlane plugin started";
    app = new QCoreApplication(argc, &argv);
    server = new TcpServer(this, this);
    connect(server, SIGNAL(setFlightLoopInterval(float)), this, SLOT(setFlightLoopInterval(float)));
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

    XPLMDataRef ref = XPLMFindDataRef(name.toLatin1());
    if(ref) {
        XPLMDataTypeID refType = XPLMGetDataRefTypes(ref);
        DataRef *dr = 0;
        if(refType & xplmType_Double) {
            dr = new DoubleDataRef(this, name, ref);
        } else if(refType & xplmType_Float) {
            dr = new FloatDataRef(this, name, ref);
        } else if(refType & xplmType_Int) {
            dr = new IntDataRef(this, name, ref);
        } else if (refType & xplmType_FloatArray) {
            dr = new FloatArrayDataRef(this, name, ref);
        } else if (refType & xplmType_IntArray) {
            dr = new IntArrayDataRef(this, name, ref);
        } else if (refType & xplmType_Data) {
            dr = new DataDataRef(this, name, ref);
        }
        if(dr) {
            dr->setSubscribers(1);
            dr->setWritable(XPLMCanWriteDataRef(ref) != 0);
            qDebug() << Q_FUNC_INFO << "Subscribed to ref " << dr->name() << ", type: " << dr->typeString() << ", writable:" << dr->isWritable();
            refs.append(dr);
            return dr;
        } else {
            qDebug() << Q_FUNC_INFO << "Dataref type " << refType << "not supported";
        }
    } else {
        qDebug() << Q_FUNC_INFO << "Can't find dataref " << name;
    }
    return 0;
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

void XPlanePlugin::keyStroke(int keyid) {
    qDebug() << Q_FUNC_INFO << keyid;

    XPLMCommandKeyStroke(keyid);
}

void XPlanePlugin::buttonPress(int buttonid) {
    qDebug() << Q_FUNC_INFO << buttonid;
    XPLMCommandButtonPress(buttonid);
}

void XPlanePlugin::buttonRelease(int buttonid) {
    qDebug() << Q_FUNC_INFO << buttonid;
    XPLMCommandButtonRelease(buttonid);
}

void XPlanePlugin::setFlightLoopInterval(float newInterval) {
    if(newInterval > 0) {
        flightLoopInterval = newInterval;
        qDebug() << Q_FUNC_INFO << "new interval" << flightLoopInterval;
    } else {
        qDebug() << Q_FUNC_INFO << "Invalid interval " << newInterval;
    }
}

void XPlanePlugin::pluginStop() {
    qDebug() << Q_FUNC_INFO;
    app->processEvents();
    delete server;
    server = 0;
    app->quit();
    app->processEvents();
    delete app;
    app = 0;
    qDeleteAll(refs);
    refs.clear();
}

void XPlanePlugin::receiveMessage(XPLMPluginID inFromWho, long inMessage, void *inParam) {
    qDebug() <<  Q_FUNC_INFO << inFromWho << inMessage;
}
