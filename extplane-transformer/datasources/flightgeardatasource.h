#ifndef FLIGHTGEARDATASOURCE_H
#define FLIGHTGEARDATASOURCE_H

#include <QObject>
#include <datarefprovider.h>
#include <QTcpSocket>
#include "../util/basictcpclient.h"
#include "datasource.h"
#include <QTimer>
#include <QMap>
#include <QVector>
#include "../extplane-server/datarefs/floatdataref.h"

class FlightGearDataSource : public DataSource
{
    Q_OBJECT

public:
    FlightGearDataSource();

public:
    virtual void connectToSource();
    DataRef *subscribeRef(QString name);
    void unsubscribeRef(DataRef *ref);
    void updateDataRef(DataRef *ref);
    void changeDataRef(DataRef *ref);
    void keyStroke(int keyid);
    void buttonPress(int buttonid);
    void buttonRelease(int buttonid);
    void command(QString &name, extplaneCommandType type);

private slots:
    void sessionOpened();
    void readLine(QString line);
    void gotNetworkError(QString errstring);

private:
    BasicTcpClient tcpClient;
    QMap<QString,QString> refMap; // Mapping of X-Plane datarefs to FG value names
    QVector<FloatDataRef*> floatRefs;
};

#endif // FLIGHTGEARDATASOURCE_H
