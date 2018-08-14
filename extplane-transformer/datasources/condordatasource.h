#ifndef CONDORDATASOURCE_H
#define CONDORDATASOURCE_H

#include <QObject>
#include <QUdpSocket>
#include <datarefs/floatdataref.h>
#include "datasource.h"

/**
 * @brief The CondorDatasource class decodes Condor (2) UDP datagrams
 *
 * Use to transform data from Condor or Condor 2 (same protocol) to ExtPlane.
 *
 * @see https://github.com/kbobrowski/CondorUDP2COM/blob/master/CondorUDP2COM.py
 */

class CondorDatasource : public DataSource
{
    Q_OBJECT

public:
    explicit CondorDatasource();
    virtual void connectToSource();
    // From DataSource:
    DataRef *subscribeRef(QString &name);
    void unsubscribeRef(DataRef *ref);
    void updateDataRef(DataRef *ref);
    void changeDataRef(DataRef *ref);
    void keyStroke(int keyid);
    void buttonPress(int buttonid);
    void buttonRelease(int buttonid);
    void command(QString &name, extplaneCommandType type);
    bool loadSituation(QString sitFileLocation);
    void addFMSEntryLatLon(QString fmsEntryLine);
    void clearAllFmsEntries();

private slots:
    void readPendingDatagrams();

private:
    void rxValue(QString &param, QString &value);
    unsigned int m_port;
    QUdpSocket m_udpSocket;
    QMap<QString, QString> refMap; // Mapping of X-Plane datarefs to Condor value names
    QList<FloatDataRef*> floatRefs; // All float datarefs used
    unsigned int m_updatesReceived;
};

#endif // CONDORDATASOURCE_H
