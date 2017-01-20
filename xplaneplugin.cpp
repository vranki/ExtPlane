#include "xplaneplugin.h"
#include "datarefs/dataref.h"
#include "datarefs/floatdataref.h"
#include "datarefs/floatarraydataref.h"
#include "datarefs/intdataref.h"
#include "datarefs/intarraydataref.h"
#include "datarefs/doubledataref.h"
#include "datarefs/datadataref.h"
#include "util/console.h"
#include "customdata/navcustomdata.h"
#include "customdata/atccustomdata.h"
#include <clocale>

XPlanePlugin::XPlanePlugin(QObject *parent) :
    QObject(parent), argc(0), argv(0), app(0), server(0), flightLoopInterval(0.31f) { // Default to 30hz
}

XPlanePlugin::~XPlanePlugin() {
    DEBUG << Q_FUNC_INFO;
}

float XPlanePlugin::flightLoop(float inElapsedSinceLastCall, float inElapsedTimeSinceLastFlightLoop,
                               int inCounter, void *inRefcon) {
    Q_UNUSED(inElapsedSinceLastCall);
    Q_UNUSED(inElapsedTimeSinceLastFlightLoop);
    Q_UNUSED(inCounter);
    Q_UNUSED(inRefcon);
    // Tell each dataref to update its value through the XPLM api
    foreach(DataRef *ref, refs) ref->updateValue();
    // Tell Qt to process it's own runloop
    app->processEvents();
    return flightLoopInterval;
}

int XPlanePlugin::pluginStart(char * outName, char * outSig, char *outDesc) {
    // Set plugin info
    INFO << "Plugin started";
    strcpy(outName, "ExtPlane");
    strcpy(outSig, "org.vranki.extplaneplugin");
    strcpy(outDesc, "Read and write X-Plane datarefs from external programs using TCP sockets.");

    // Init application and server
    app = new QCoreApplication(argc, &argv);
    setlocale(LC_NUMERIC, "C"); // See http://stackoverflow.com/questions/25661295/why-does-qcoreapplication-call-setlocalelc-all-by-default-on-unix-linux

    server = new TcpServer(this, this);
    connect(server, SIGNAL(setFlightLoopInterval(float)), this, SLOT(setFlightLoopInterval(float)));

    // Register the nav custom data accessors
    XPLMRegisterDataAccessor("extplane/navdata/5km",
                                                 xplmType_Data,                                 // The types we support
                                                 0,                                             // Writable
                                                 NULL, NULL,                                    // Integer accessors
                                                 NULL, NULL,                                    // Float accessors
                                                 NULL, NULL,                                    // Doubles accessors
                                                 NULL, NULL,                                    // Int array accessors
                                                 NULL, NULL,                                    // Float array accessors
                                                 NavCustomData::DataCallback_5km, NULL,         // Raw data accessors
                                                 NULL, NULL);                                   // Refcons not used
    XPLMRegisterDataAccessor("extplane/navdata/20km",
                                                 xplmType_Data,                                 // The types we support
                                                 0,                                             // Writable
                                                 NULL, NULL,                                    // Integer accessors
                                                 NULL, NULL,                                    // Float accessors
                                                 NULL, NULL,                                    // Doubles accessors
                                                 NULL, NULL,                                    // Int array accessors
                                                 NULL, NULL,                                    // Float array accessors
                                                 NavCustomData::DataCallback_20km, NULL,        // Raw data accessors
                                                 NULL, NULL);                                   // Refcons not used
    XPLMRegisterDataAccessor("extplane/navdata/100km",
                                                 xplmType_Data,                                 // The types we support
                                                 0,                                             // Writable
                                                 NULL, NULL,                                    // Integer accessors
                                                 NULL, NULL,                                    // Float accessors
                                                 NULL, NULL,                                    // Doubles accessors
                                                 NULL, NULL,                                    // Int array accessors
                                                 NULL, NULL,                                    // Float array accessors
                                                 NavCustomData::DataCallback_100km, NULL,       // Raw data accessors
                                                 NULL, NULL);                                   // Refcons not used
    XPLMRegisterDataAccessor("extplane/atc/124thatc/latest",
                                                 xplmType_Data,                                 // The types we support
                                                 0,                                             // Writable
                                                 NULL, NULL,                                    // Integer accessors
                                                 NULL, NULL,                                    // Float accessors
                                                 NULL, NULL,                                    // Doubles accessors
                                                 NULL, NULL,                                    // Int array accessors
                                                 NULL, NULL,                                    // Float array accessors
                                                 ATCCustomData::DataCallback, NULL,             // Raw data accessors
                                                 NULL, NULL);

    app->processEvents();
    return 1;
}

DataRef* XPlanePlugin::subscribeRef(QString name) {
    DEBUG << name;

    // Search in list of already subscribed datarefs - if found return that
    foreach(DataRef *ref, refs) {
        if(ref->name()==name) {
            DEBUG << "Already subscribed to " << name;
            ref->setSubscribers(ref->subscribers() + 1);
            return ref;
        }
    }

    // Not yet subscribed - create a new dataref
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
            DEBUG << "Subscribed to ref " << dr->name() << ", type: " << dr->typeString() << ", writable:" << dr->isWritable();
            refs.append(dr);
            return dr;
        } else {
            INFO << "Dataref type " << refType << "not supported";
        }
    } else {
        INFO << "Can't find dataref " << name;
    }
    return 0;
}

void XPlanePlugin::unsubscribeRef(DataRef *ref) {
    Q_ASSERT(refs.contains(ref));
    DEBUG << ref->name() << ref->subscribers();
    ref->setSubscribers(ref->subscribers() - 1);
    if(ref->subscribers() == 0) {
        refs.removeOne(ref);
        DEBUG << "Ref " << ref->name() << " not subscribed by anyone - removing.";
        ref->deleteLater();
    }
}

void XPlanePlugin::keyStroke(int keyid) {
    DEBUG << keyid;
    XPLMCommandKeyStroke(keyid);
}

void XPlanePlugin::buttonPress(int buttonid) {
    DEBUG << buttonid;
    XPLMCommandButtonPress(buttonid);
}

void XPlanePlugin::buttonRelease(int buttonid) {
    DEBUG << buttonid;
    XPLMCommandButtonRelease(buttonid);
}

void XPlanePlugin::setFlightLoopInterval(float newInterval) {
    if(newInterval > 0) {
        flightLoopInterval = newInterval;
        DEBUG << "New interval" << flightLoopInterval;
    } else {
        DEBUG << "Invalid interval " << newInterval;
    }
}

void XPlanePlugin::pluginStop() {
    DEBUG;
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
    Q_UNUSED(inParam);

    DEBUG << inFromWho << inMessage;
}
