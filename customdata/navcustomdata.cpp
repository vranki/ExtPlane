#include "navcustomdata.h"

#include <QList>
#include <QTime>
#include <qmath.h>
#include <QDebug>
#include <QThread>

#include "util/console.h"

// The average size of a navdata line in bytes
#define NAVCUSTOMDATA_NAVADATA_SIZE_B 80

NavCustomData *_navCustomData = NULL;

inline QString GetLatLonKey(double lat, double lon) {
    int latSector = qFloor(lat);
    int lonSector = qFloor(lon);
    return QString("%1_%2").arg(latSector).arg(lonSector);
}


NavCustomData::NavCustomData(QObject *parent) :
    QObject(parent)
{
    INFO << "Creating new NavCustomData...";

    // Init
    int total = 0;
    QTime timer;
    timer.restart();

    // Loop through entire database
    XPLMNavRef navAid = XPLMGetFirstNavAid();
    while(navAid != XPLM_NAV_NOT_FOUND) {
        // Get next
        total++;
        navAid = XPLMGetNextNavAid(navAid);

        // Get nav aid info
        float lon,lat;
        XPLMGetNavAidInfo(navAid,NULL,&lat,&lon,NULL,NULL,NULL,NULL,NULL,NULL);

        // Create key and register the navaidref
        // Note: we could store the navaid info here if needed, but it doesn't
        // seem necessary as the lookup XPLMGetNavAidInfo is cheap.
        QString navKey = GetLatLonKey(lat,lon);
        _data.insert(navKey, navAid);
    }

    INFO << "Finished creating NavCustomData: " << total << "nav aids," << timer.elapsed() << "ms";
}


int NavCustomData::getNavDataForSector(int lat, int lon, QString *retData, double planeX, double planeZ, double radius, int maxPacketSize) {
    return getNavDataForSector(GetLatLonKey(lat,lon),retData,planeX,planeZ,radius,maxPacketSize);
}


int NavCustomData::getNavDataForSector(QString sector, QString *retData, double planeX, double planeZ, double radius, int maxPacketSize) {
    // Loop all navaids for this sector
    DEBUG << "scanning sector" << sector;
    int included = 0;
    int discarded = 0;
    QMultiMap<QString, XPLMNavRef>::iterator it = _navCustomData->_data.find(sector);
    while (it != _navCustomData->_data.end() && it.key() == sector) {

        // Get navaid info
        XPLMNavRef navAid = it.value();
        float lon=0,lat=0,height=0,heading=0;
        int frequency = 0;
        char id[32];
        char name[256];
        XPLMNavType type;
        XPLMGetNavAidInfo(navAid,&type,&lat,&lon,&height,&frequency,&heading,id,name,NULL);

        // Get local cords
        double localX,localY,localZ;
        XPLMWorldToLocal(lat,lon,height,&localX,&localY,&localZ);

        // Check if distance to navaid is within our radius
        double dist = qSqrt( (localX - planeX)*(localX - planeX) + (localZ - planeZ)*(localZ - planeZ) );
        if(dist < radius) {

            // Create the data line for this navaid. A data line looks like this:
            // ID:TYPE:LAT:LON:LOCALX:LOCALZ:HEIGHT:FREQ:HEAD:NAME
            // KSEA:1:47.4475:-122.308:14461.6:-211.06:131.978:0:0:Seattle Tacoma Intl
            // IBFI:8:47.5155:-122.29:15776:-277.859:6.09532:11090:150.243:KBFI 13R ILS-cat-I
            // JUGBA:512:48.0517:-122.289:15717.4:-1094.2:0:0:0:JUGBA
            // WA53:1:47.604:-122.323:13271.5:-368.349:96.012:0:0:[H] Harborview Medical Center
            // ----:64:48.0528:-122.289:15719.3:-1096.42:172.212:0:196.948:KPAE 16R OM
            QString nameCleaned = QString(name).replace(':',' ');
            QString navDataLine = QString("%1:%2:%3:%4:%5:%6:%7:%8:%9:%10\n").arg(id).arg(type).arg(lat).arg(lon).arg(localX).arg(localZ).arg(height).arg(frequency).arg(heading).arg(nameCleaned);
            if(retData->length() + navDataLine.length() < maxPacketSize) {
                retData->append(navDataLine);
                included++;
            } else {
                discarded++;
            }
        }

        // Move to next
        ++it;
    }

    DEBUG << "included" << included << "nav aids";
    if(discarded > 0) {
        DEBUG << "discarded" << discarded << "nav aids (did not fit packet)";
    }

    return included;
}

int NavCustomData::DataCallback_5km(void *inRefcon, void *outValue, int inOffset, int inMaxLength) {
    return DataCallback_Common(inRefcon, outValue, inOffset, inMaxLength, 5000, 50);
}

int NavCustomData::DataCallback_20km(void *inRefcon, void *outValue, int inOffset, int inMaxLength) {
    return DataCallback_Common(inRefcon, outValue, inOffset, inMaxLength, 20000, 100);
}

int NavCustomData::DataCallback_100km(void *inRefcon, void *outValue, int inOffset, int inMaxLength) {
    return DataCallback_Common(inRefcon, outValue, inOffset, inMaxLength, 100000, 300);
}

int NavCustomData::DataCallback_Common(void *inRefcon, void *outValue, int inOffset, int inMaxLength, double radius, int expectedNavAids) {

    // Make sure the NavCustomData is initialized. This will happen once when the plugin is reloaded
    // upon the first call to the dataref
    if(_navCustomData == NULL) {
        _navCustomData = new NavCustomData();
    }

    // Init
    int packetSize = NAVCUSTOMDATA_NAVADATA_SIZE_B*expectedNavAids;
    QString retData = "";

    if(outValue != NULL) {

        // Book-keeping
        QTime timer;
        timer.restart();

        // Get current lat lon
        double planeLat = XPLMGetDatad(XPLMFindDataRef("sim/flightmodel/position/latitude"));
        double planeLon = XPLMGetDatad(XPLMFindDataRef("sim/flightmodel/position/longitude"));
        double planeX = XPLMGetDatad(XPLMFindDataRef("sim/flightmodel/position/local_x"));
        double planeZ = XPLMGetDatad(XPLMFindDataRef("sim/flightmodel/position/local_z"));

        // Artificially add our own aircraft,
        //QString planeDataLine = QString("%1:%2:%3:%4:%5:%6:%7:%8:%9:%10\n").arg("PLANE").arg(0).arg(planeLat).arg(planeLon).arg(planeX).arg(planeZ).arg(0).arg(0).arg(0).arg("");
        //retData.append(planeDataLine);

        // Now add all nav aids using a 3x3 grid around our plane coordinates
        int count = 0;
        for(int x = -1; x <= 1; x++) {
            for(int y = -1; y <= 1; y++) {
                count += _navCustomData->getNavDataForSector(planeLat+x, planeLon+y, &retData, planeX, planeZ, radius, packetSize);
            }
        }

        DEBUG << "plane" << GetLatLonKey(planeLat,planeLon) << count << "ms" << timer.elapsed();

        // Copy to outdata
        // TODO: This should be refactored out and we should write directly to the outdata for more optimal performance
        for(int i = 0; i < packetSize; i++) {
            if(i >= retData.length()) {
                ((char*)outValue)[i] = 0;
            } else {
                ((char*)outValue)[i] = retData[i].toLatin1();
            }
        }
    }

    return packetSize;
}
