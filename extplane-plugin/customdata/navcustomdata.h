#ifndef NAVCUSTOMDATA_H
#define NAVCUSTOMDATA_H

#include <QObject>
#include <QMap>
#include <QMultiMap>
#include <QList>
#include <QString>

#include "XPLMNavigation.h"
#include "XPLMGraphics.h"
#include "XPLMDataAccess.h"
#include "XPLMUtilities.h"

/**
  * This custom data accessor compiles a list of navigation data provided by X-Plane.
  * It allows ExtPlane clients to build up a map or radar using the data provided.
  * Unfortunately X-PLane doesnt provide any good APIs for doing this, so this data accessor
  * aims to help intergration of such features by creating an easy-to-use data format.
  *
  * CURRENTLY IN TESTING!
  *
  * Notes:
  *
  * A fresh install of X-PLane has about 170'000 nav aids. This grows as custom scenery is
  * added.
  * Different strategies have been tested using different XPLM APIs. Here are the findings:
  *   - Get closeby within radius using XPLMGetNextNavAid to loop all navaids
  *     Pros: robust and easy solution; cons: expensive (15-30ms) and doesnt scale
  *   - Get closeby through looping a grid surrounding the aircraft and calling
  *     XPLMFindNavAid.
  *     Pros: none, cons: 512ms (seems like X-Plane loops the list each time
  *   - Compile a lookup table based on navaid coordinates at startup (just once), then just lookup by
  *     sector according to plane's coordinates. This seems the only easy viable solution.
  *     Pros: fast lookups for relavant sector (<1ms), cons: takes time upon startup (600ms).
  * These tests where done on a MacBookPro Retina
  */
class NavCustomData : public QObject
{
    Q_OBJECT
public:
    explicit NavCustomData(QObject *parent = 0);
    static int DataCallback_5km(void *inRefcon, void *outValue, int inOffset, int inMaxLength);
    static int DataCallback_20km(void *inRefcon, void *outValue, int inOffset, int inMaxLength);
    static int DataCallback_100km(void *inRefcon, void *outValue, int inOffset, int inMaxLength);
    static int DataCallback_Common(void *inRefcon, void *outValue, int inOffset, int inMaxLength, double radius, int expectedNavAids);
    int getNavDataForSector(int lat, int lon, QString *retData, double planeX, double planeY, double radius, int maxPacketSize);
    int getNavDataForSector(QString sector, QString *retData, double planeX, double planeZ, double radius, int maxPacketSize);

signals:
    
public slots:

private:
    QMultiMap<QString,XPLMNavRef> _data;    // A multi-map to store coordinate sectors. Newer Qt versions uses a red-black tree so this is pretty efficient as far as lookups as well as insertion.
    
};

#endif // NAVCUSTOMDATA_H
