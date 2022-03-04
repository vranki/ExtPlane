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
#include <cstring>
#include <XPLMUtilities.h>

#ifdef WITH_MQTT
#include "../mqttpublisher/mqttpublisher.h"
#endif

XPlanePlugin::XPlanePlugin(QObject *parent) : QObject(parent) { }

XPlanePlugin::~XPlanePlugin() { }

float XPlanePlugin::flightLoop(float inElapsedSinceLastCall,
                               float inElapsedTimeSinceLastFlightLoop,
                               int inCounter,
                               void *inRefcon) {
    Q_UNUSED(inElapsedSinceLastCall)
    Q_UNUSED(inElapsedTimeSinceLastFlightLoop)
    Q_UNUSED(inCounter)
    Q_UNUSED(inRefcon)
    // Tell each dataref to update its value through the XPLM api
    for(DataRef *ref : m_refs) updateDataRef(ref);
    if(m_server) m_server->flightLoop();
    // Tell Qt to process it's own runloop
    m_app->processEvents();
#ifdef WITH_MQTT
    if(m_mqttPublisher) {
        m_mqttPublisher->poll();
    }
#endif
    return m_flightLoopInterval;
}

int XPlanePlugin::pluginStart(char * outName, char * outSig, char *outDesc) {
    // Set plugin info
    INFO << "Plugin started";
    std::strcpy(outName, "ExtPlane");
    std::strcpy(outSig, "org.vranki.extplaneplugin");
    std::strcpy(outDesc, "Read and write X-Plane datarefs from external programs on TCP port " EXTPLANE_PORT_STR " with protocol " EXTPLANE_PROTOCOL_STR " version " EXTPLANE_VERSION_STR);

    Q_ASSERT(!m_app);
    // Init application and server
    m_app = new QCoreApplication(argc, &argv);
    setlocale(LC_NUMERIC, "C"); // See http://stackoverflow.com/questions/25661295/why-does-qcoreapplication-call-setlocalelc-all-by-default-on-unix-linux

    return 1;
}

DataRef* XPlanePlugin::subscribeRef(QString &name) {
    // Search in list of already subscribed datarefs - if found return that
    for(DataRef *ref : m_refs) {
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
            m_refs.insert(dr);
            return dr;
        } else {
            m_server->extplaneWarning(QString("Dataref type %1 not supported").arg(refType));
        }
    } else {
        m_server->extplaneWarning(QString("Can't find dataref %1").arg(name));
    }
    return nullptr;
}

void XPlanePlugin::unsubscribeRef(DataRef *ref) {
    Q_ASSERT(m_refs.find(ref) != m_refs.end());

    ref->setSubscriberCount(ref->subscriberCount() - 1);
    if(ref->subscriberCount() == 0) {
        m_refs.erase(ref);
        DEBUG << "Ref " << ref->name() << " not subscribed by anyone - removing.";
        ref->deleteLater();
    }
}

// Called for each ref on every flight loop
void XPlanePlugin::updateDataRef(DataRef *ref) {
    Q_ASSERT(ref);

    switch (ref->type()) {
    case extplaneRefTypeFloat:
    {
        float newValue = XPLMGetDataf(ref->ref());
        qobject_cast<FloatDataRef*>(ref)->updateValue(newValue);
        break;
    }
    case extplaneRefTypeFloatArray:
    {
        FloatArrayDataRef *faRef = qobject_cast<FloatArrayDataRef*>(ref);
        int arrayLength = faRef->value().size();
        if(arrayLength == 0) {
            arrayLength = XPLMGetDatavf(faRef->ref(), nullptr, 0, 0);
            faRef->setLength(arrayLength);
        }
        int valuesCopied = XPLMGetDatavf(faRef->ref(), faRef->valueArray(), 0, arrayLength);
        Q_ASSERT(valuesCopied == arrayLength);
        faRef->updateValue();
        break;
    }
    case extplaneRefTypeIntArray:
    {
        IntArrayDataRef *iaRef = qobject_cast<IntArrayDataRef*>(ref);
        int arrayLength = iaRef->value().size();
        if(arrayLength <= 0) {
            arrayLength = XPLMGetDatavi(iaRef->ref(), nullptr, 0, 0);
            iaRef->setLength(arrayLength);
        }
        int valuesCopied = XPLMGetDatavi(iaRef->ref(), iaRef->valueArray(), 0, arrayLength);
        Q_ASSERT(valuesCopied == arrayLength);
        iaRef->updateValue();
        break;
    }
    case extplaneRefTypeInt:
    {
        int newValue = XPLMGetDatai(ref->ref());
        qobject_cast<IntDataRef*>(ref)->updateValue(newValue);
        break;
    }
    case extplaneRefTypeDouble:
    {
        double newValue = XPLMGetDatad(ref->ref());
        qobject_cast<DoubleDataRef*>(ref)->updateValue(newValue);
        break;
    }
    case extplaneRefTypeData:
    {
        DataDataRef *bRef = qobject_cast<DataDataRef*>(ref);
        Q_ASSERT(bRef);
        int arrayLength = XPLMGetDatab(ref->ref(), nullptr, 0, 0);
        bRef->setLength(arrayLength);
        int valuesCopied = XPLMGetDatab(ref->ref(), bRef->newValue().data(), 0, arrayLength);
        Q_ASSERT(valuesCopied == arrayLength);
        bRef->updateValue();
        break;
    }
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
        m_server->extplaneWarning(QString("Tried to write read-only dataref %1").arg(ref->name()));
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
        while(!faRef->changedIndices->empty()) {
            XPLMSetDatavf(ref->ref(), faRef->valueArray() + faRef->changedIndices->front().lower,
             faRef->changedIndices->front().lower,
             faRef->changedIndices->front().upper - faRef->changedIndices->front().lower + 1
            );
            faRef->changedIndices->pop_front();
        }
        break;
    }
    case extplaneRefTypeIntArray:
    {
        IntArrayDataRef *iaRef = qobject_cast<IntArrayDataRef*>(ref);
        while(!iaRef->changedIndices.empty()) {
            XPLMSetDatavi(ref->ref(), iaRef->valueArray() + iaRef->changedIndices.front().lower,
             iaRef->changedIndices.front().lower,
             iaRef->changedIndices.front().upper - iaRef->changedIndices.front().lower + 1
            );
            iaRef->changedIndices.pop_front();
        }
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
    case extplaneRefTypeData:
    {
        QString value = ref->valueString();
        Q_ASSERT(value.count("\"") >= 2);
        value = value.mid(value.indexOf("\"") + 1);
        value = value.mid(0, value.lastIndexOf("\""));
        // Check ref length
        int length = XPLMGetDatab(ref->ref(), nullptr, 0, 0);
        // Create a bytearray of correct size and fill with 0's
        QByteArray valueBa(length, 0);
        valueBa.insert(0, value.toUtf8());
        // Make sure size is still correct
        valueBa.resize(length);
        XPLMSetDatab(ref->ref(), valueBa.data(), 0, valueBa.length());
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
        m_server->extplaneWarning(QString("Command %1 not found").arg(name));
    }

}

void XPlanePlugin::setFlightLoopInterval(float newInterval) {
    m_flightLoopInterval = newInterval;
    DEBUG << "New interval" << m_flightLoopInterval;
}

QString XPlanePlugin::refNameWithoutModifiers(QString &original) {
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

/**
 * @brief XPlanePlugin::addFMSEntryLatLon
 * @param fmsEntryLine - string in format "id,latitude,longitude,altitude"
 * example: "1,50.0267,8.51,198"
 */
void XPlanePlugin::addFMSEntryLatLon(QString fmsEntryLine){
    //verify if string is in valid format
    int commaCount = fmsEntryLine.count(QLatin1Char(','));
    if(commaCount != 3){
        return;
    }

    QStringList params = fmsEntryLine.split(",", Qt::SkipEmptyParts);
    int id = params.value(0).toInt();
    float lat = params.value(1).toFloat();
    float lon = params.value(2).toFloat();
    int alt = params.value(3).toInt();

    XPLMSetFMSEntryLatLon(id, lat, lon, alt);
}

/**
 * @brief XPlanePlugin::clearAllFmsEntries
 * removes all entries inserted to FMS
 */
void XPlanePlugin::clearAllFmsEntries(){
    int count = XPLMCountFMSEntries();
    for(int i = 0; i < count;i++){
        XPLMClearFMSEntry(i);
    }
}
/**
 * @brief XPlanePlugin::setDestinationFmsEntry
 * @param index entry the FMS is flying the aircraft toward.
 */
void XPlanePlugin::setDestinationFmsEntry(int index) {
    XPLMSetDestinationFMSEntry(index);
}


void XPlanePlugin::pluginStop() {
    DEBUG;
    Q_ASSERT(m_app);
    delete m_app;
    m_app = nullptr;
}

int XPlanePlugin::pluginEnable() {
    Q_ASSERT(!m_server);
    m_server = new TcpServer(this, this);
    connect(m_server, &TcpServer::setFlightLoopInterval, this, &XPlanePlugin::setFlightLoopInterval);

    // Log that we have started
    XPLMDebugString ("ExtPlane listening on TCP port " EXTPLANE_PORT_STR " with protocol " EXTPLANE_PROTOCOL_STR " version " EXTPLANE_VERSION_STR "\n");

    // Register the nav custom data accessors
    XPLMRegisterDataAccessor("extplane/navdata/5km",
                             xplmType_Data,                                 // The types we support
                             0,                                             // Writable
                             nullptr, nullptr,                                    // Integer accessors
                             nullptr, nullptr,                                    // Float accessors
                             nullptr, nullptr,                                    // Doubles accessors
                             nullptr, nullptr,                                    // Int array accessors
                             nullptr, nullptr,                                    // Float array accessors
                             NavCustomData::DataCallback_5km, nullptr,         // Raw data accessors
                             nullptr, nullptr);                                   // Refcons not used
    XPLMRegisterDataAccessor("extplane/navdata/20km",
                             xplmType_Data,                                 // The types we support
                             0,                                             // Writable
                             nullptr, nullptr,                                    // Integer accessors
                             nullptr, nullptr,                                    // Float accessors
                             nullptr, nullptr,                                    // Doubles accessors
                             nullptr, nullptr,                                    // Int array accessors
                             nullptr, nullptr,                                    // Float array accessors
                             NavCustomData::DataCallback_20km, nullptr,        // Raw data accessors
                             nullptr, nullptr);                                   // Refcons not used
    XPLMRegisterDataAccessor("extplane/navdata/100km",
                             xplmType_Data,                                 // The types we support
                             0,                                             // Writable
                             nullptr, nullptr,                                    // Integer accessors
                             nullptr, nullptr,                                    // Float accessors
                             nullptr, nullptr,                                    // Doubles accessors
                             nullptr, nullptr,                                    // Int array accessors
                             nullptr, nullptr,                                    // Float array accessors
                             NavCustomData::DataCallback_100km, nullptr,       // Raw data accessors
                             nullptr, nullptr);                                   // Refcons not used
    XPLMRegisterDataAccessor("extplane/atc/124thatc/latest",
                             xplmType_Data,                                 // The types we support
                             0,                                             // Writable
                             nullptr, nullptr,                                    // Integer accessors
                             nullptr, nullptr,                                    // Float accessors
                             nullptr, nullptr,                                    // Doubles accessors
                             nullptr, nullptr,                                    // Int array accessors
                             nullptr, nullptr,                                    // Float array accessors
                             ATCCustomData::DataCallback, nullptr,             // Raw data accessors
                             nullptr, nullptr);
#ifdef WITH_MQTT
    m_mqttPublisher = new MQTTPublisher(this, this);
    m_mqttPublisher->start();
#endif
    m_menu_container_idx = XPLMAppendMenuItem(XPLMFindPluginsMenu(), "ExtPlane", nullptr, 0);
    m_menu_id = XPLMCreateMenu("ExtPlane", XPLMFindPluginsMenu(), m_menu_container_idx, nullptr, nullptr);
    QString menuItem = QString("Listening on TCP port %1 with protocol %2 version %3. MQTT output %4. No GUI yet.").arg(EXTPLANE_PORT_STR).arg(EXTPLANE_PROTOCOL_STR).arg(EXTPLANE_VERSION_STR).arg(m_mqttPublisher ? "Enabled" : "Disabled");
    XPLMAppendMenuItem(m_menu_id, qPrintable(menuItem), nullptr, 1);

    return 1;
}

void XPlanePlugin::pluginDisable() {
    XPLMRemoveMenuItem(XPLMFindPluginsMenu(), m_menu_container_idx);
    m_menu_container_idx = -1;
    XPLMDestroyMenu(m_menu_id);
    m_menu_id = nullptr;

    if(m_mqttPublisher) {
#ifdef WITH_MQTT
        m_mqttPublisher->stop();
        delete m_mqttPublisher;
#endif
        m_mqttPublisher = nullptr;
    }

    if(m_server)
        m_server->disconnectClients();
    delete m_server;

    Q_ASSERT(m_refs.empty());
    m_server = nullptr;
}
