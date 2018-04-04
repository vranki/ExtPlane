#include "xplaneplugin.h"
#include "../extplane-server/datarefs/dataref.h"
#include "../extplane-server/datarefs/floatdataref.h"
#include "../extplane-server/datarefs/floatarraydataref.h"
#include "../extplane-server/datarefs/intdataref.h"
#include "../extplane-server/datarefs/intarraydataref.h"
#include "../extplane-server/datarefs/doubledataref.h"
#include "../extplane-server/datarefs/datadataref.h"
#include <console.h>
#include "customdata/navcustomdata.h"
#include "customdata/atccustomdata.h"
#include <clocale>
#include <XPLMUtilities.h>

XPlanePlugin::XPlanePlugin(QObject *parent) : QObject(parent)
  , argc(0)
  , argv(nullptr)
  , app(nullptr)
  , server(nullptr)
  , flightLoopInterval(1.0f / 30.f) // Default to 30hz
{ }

XPlanePlugin::~XPlanePlugin() {
    DEBUG << Q_FUNC_INFO;
}

float XPlanePlugin::flightLoop(float inElapsedSinceLastCall,
                               float inElapsedTimeSinceLastFlightLoop,
                               int inCounter,
                               void *inRefcon) {
    Q_UNUSED(inElapsedSinceLastCall);
    Q_UNUSED(inElapsedTimeSinceLastFlightLoop);
    Q_UNUSED(inCounter);
    Q_UNUSED(inRefcon);
    // Tell each dataref to update its value through the XPLM api
    for(DataRef *ref : refs) updateDataRef(ref);
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

DataRef* XPlanePlugin::subscribeRef(QString &name) {
    DEBUG << name;

    // Search in list of already subscribed datarefs - if found return that
    for(DataRef *ref : refs) {
        if(ref->name() == name) {
            DEBUG << "Already subscribed to " << name;
            ref->setSubscriberCount(ref->subscriberCount() + 1);
            return ref;
        }
    }

    // Not yet subscribed - create a new dataref
    QString realName = refNameWithoutModifiers(name);
    XPLMDataRef ref = XPLMFindDataRef(realName.toLatin1());
    if(ref) {
        XPLMDataTypeID refType = XPLMGetDataRefTypes(ref);
        DataRef *dr = nullptr;
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
            dr->setSubscriberCount(1);
            dr->setWritable(XPLMCanWriteDataRef(ref));
            DEBUG << "Subscribed to ref " << dr->name()
                  << ", type: " << dr->typeString()
                  << ", writable:" << dr->isWritable();
            refs.append(dr);
            return dr;
        } else {
            INFO << "Dataref type " << refType << "not supported";
        }
    } else {
        INFO << "Can't find dataref " << name;
    }
    return nullptr;
}

void XPlanePlugin::unsubscribeRef(DataRef *ref) {
    Q_ASSERT(refs.contains(ref));
    DEBUG << ref->name() << ref->subscriberCount();

    ref->setSubscriberCount(ref->subscriberCount() - 1);
    if(ref->subscriberCount() == 0) {
        refs.removeOne(ref);
        DEBUG << "Ref " << ref->name() << " not subscribed by anyone - removing.";
        ref->deleteLater();
    }
}

// Called for each ref on every flight loop
void XPlanePlugin::updateDataRef(DataRef *ref)
{
    Q_ASSERT(ref);

    switch (ref->type()) {
    case extplaneRefTypeFloat:
    {
        float newValue = XPLMGetDataf(ref->ref());
        qobject_cast<FloatDataRef*>(ref)->updateValue(newValue);
        break;
    };
    case extplaneRefTypeFloatArray:
    {
        FloatArrayDataRef *faRef = qobject_cast<FloatArrayDataRef*>(ref);
        int arrayLength = faRef->value().length();
        if(arrayLength == 0) {
            arrayLength = XPLMGetDatavf(faRef->ref(), NULL, 0, 0);
            faRef->setLength(arrayLength);
        }
        int valuesCopied = XPLMGetDatavf(faRef->ref(), faRef->valueArray(), 0, arrayLength);
        Q_ASSERT(valuesCopied == arrayLength);
        faRef->updateValue();
        break;
    };
    case extplaneRefTypeIntArray:
    {
        IntArrayDataRef *iaRef = qobject_cast<IntArrayDataRef*>(ref);
        int arrayLength = iaRef->value().length();
        if(arrayLength <= 0) {
            arrayLength = XPLMGetDatavi(iaRef->ref(), NULL, 0, 0);
            iaRef->setLength(arrayLength);
        }
        int valuesCopied = XPLMGetDatavi(iaRef->ref(), iaRef->valueArray(), 0, arrayLength);
        Q_ASSERT(valuesCopied == arrayLength);
        iaRef->updateValue();
        break;
    };
    case extplaneRefTypeInt:
    {
        int newValue = XPLMGetDatai(ref->ref());
        qobject_cast<IntDataRef*>(ref)->updateValue(newValue);
        break;
    };
    case extplaneRefTypeDouble:
    {
        double newValue = XPLMGetDatad(ref->ref());
        qobject_cast<DoubleDataRef*>(ref)->updateValue(newValue);
        break;
    };
    case extplaneRefTypeData:
    {
        DataDataRef *bRef = qobject_cast<DataDataRef*>(ref);
        Q_ASSERT(bRef);
        int arrayLength = XPLMGetDatab(ref->ref(), NULL, 0, 0);
        bRef->setLength(arrayLength);
        int valuesCopied = XPLMGetDatab(ref->ref(), bRef->newValue().data(), 0, arrayLength);
        Q_ASSERT(valuesCopied == arrayLength);
        bRef->updateValue();
        break;
    };
    default:
        break;
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

void XPlanePlugin::changeDataRef(DataRef *ref)
{
    if(!ref->isWritable()) {
        INFO << "Tried to write read-only dataref" << ref->name();
        return;
    }

    switch (ref->type()) {
    case extplaneRefTypeFloat:
    {
        XPLMSetDataf(ref->ref(), qobject_cast<FloatDataRef*>(ref)->value());
        break;
    }
    case extplaneRefTypeFloatArray:
    {
        FloatArrayDataRef *faRef = qobject_cast<FloatArrayDataRef*>(ref);
        XPLMSetDatavf(ref->ref(), faRef->valueArray(), 0, faRef->value().length());
        break;
    }
    case extplaneRefTypeIntArray:
    {
        IntArrayDataRef *iaRef = qobject_cast<IntArrayDataRef*>(ref);
        XPLMSetDatavi(ref->ref(), iaRef->valueArray(), 0, iaRef->value().length());
        break;
    }
    case extplaneRefTypeInt:
    {
        XPLMSetDatai(ref->ref(), qobject_cast<IntDataRef*>(ref)->value());
        break;
    }
    case extplaneRefTypeDouble:
    {
        XPLMSetDatad(ref->ref(), qobject_cast<DoubleDataRef*>(ref)->value());
        break;
    }
    default:
        break;
    }
}

void XPlanePlugin::command(QString &name, extplaneCommandType type)
{
    XPLMCommandRef cmdRef = XPLMFindCommand(name.toUtf8().constData());
    if (cmdRef) {
        switch (type) {
        case extplaneCommandTypeOnce:
            XPLMCommandOnce(cmdRef);
            break;
        case extplaneCommandTypeBegin:
            XPLMCommandBegin(cmdRef);
            break;
        case extplaneCommandTypeEnd:
            XPLMCommandEnd(cmdRef);
            break;
        default:
            break;
        }
    } else {
        INFO << "Command" << name << "not found";
    }

}

void XPlanePlugin::setFlightLoopInterval(float newInterval) {
    if(newInterval > 0) {
        flightLoopInterval = newInterval;
        DEBUG << "New interval" << flightLoopInterval;
    } else {
        DEBUG << "Invalid interval " << newInterval;
    }
}

QString XPlanePlugin::refNameWithoutModifiers(QString &original)
{
    return original.contains(":") ? original.left(original.indexOf(":")) : original;
}

/**
 * @brief XPlanePlugin::loadSituation
 * @param name :  situation file location -
 * relative to XPlane root folder, e.g. Output/situations/XXX.sit
 */
bool XPlanePlugin::loadSituation(QString sitFileLocation) {

    // Remove quotes from filename
    sitFileLocation = sitFileLocation.replace("\"", "");

    // XPLMLoadDataFile's return value is not documented, assuming it returns
    // 1 on success and 0 on fail. TODO: Check this.
    return XPLMLoadDataFile(xplm_DataFile_Situation, sitFileLocation.toUtf8().data());
}

void XPlanePlugin::pluginStop() {
    DEBUG;
    app->processEvents();
    delete server;
    server = nullptr;
    app->quit();
    app->processEvents();
    delete app;
    app = nullptr;
    qDeleteAll(refs);
    refs.clear();
}

void XPlanePlugin::receiveMessage(XPLMPluginID inFromWho, long inMessage, void *inParam) {
    Q_UNUSED(inParam);

    DEBUG << inFromWho << inMessage;
}
