#include "fixedsimulateddataref.h"

FixedSimulatedDataRef::FixedSimulatedDataRef(QObject *parent, QString value, QString refName) :
    SimulatedDataRef(parent,0,0,1,false,0,refName), _value(value)
{
}

void FixedSimulatedDataRef::tickTime(double dt, int total) {
    Q_UNUSED(dt);
    Q_UNUSED(total);
}

void FixedSimulatedDataRef::changeTimeout() {
    myClientRef.updateValue(_value);
}





NavDataFixedSimulatedDataRef::NavDataFixedSimulatedDataRef(QObject *parent, float range, QString refName) :
    FixedSimulatedDataRef(parent,"",refName)
{
    _value = "";
    for(int i = 0; i < 100; i++) {
        // Create the data line for this navaid. A data line looks like this:
        // ID:TYPE:LAT:LON:LOCALX:LOCALY:HEIGHT:FREQ:HEAD:NAME
        // KSEA:1:47.4475:-122.308:14461.6:-211.06:131.978:0:0:Seattle Tacoma Intl
        // IBFI:8:47.5155:-122.29:15776:-277.859:6.09532:11090:150.243:KBFI 13R ILS-cat-I
        // JUGBA:512:48.0517:-122.289:15717.4:-1094.2:0:0:0:JUGBA
        // WA53:1:47.604:-122.323:13271.5:-368.349:96.012:0:0:[H] Harborview Medical Center
        // ----:64:48.0528:-122.289:15719.3:-1096.42:172.212:0:196.948:KPAE 16R OM
        double localX = (-((int)range*10) + qrand() % ((int)range*10*2)) / 10.0;
        double localY = (-((int)range*10) + qrand() % ((int)range*10*2)) / 10.0;
        double lat = 47.4475;
        double lon = 8.0001;
        int typeRand = qrand() % 5;
        QString type = "1";
        if(typeRand == 0) type = "1";
        if(typeRand == 1) type = "8";
        if(typeRand == 2) type = "512";
        if(typeRand == 3) type = "64";
        int idRand = qrand() % 5;
        QString id = "----";
        if(idRand == 0) id = "KSEA";
        if(idRand == 1) id = "IBFI";
        if(idRand == 2) id = "JUGBA";
        if(idRand == 3) id = "WA53";
        QString name = "Seattle Tacoma Intl";
        if(idRand == 0) name = "Zurich Intl";
        if(idRand == 1) name = "John F. Keneddy";
        if(idRand == 2) name = "La Guardia";
        if(idRand == 3) name = "Really Long Name Intl";
        QString dataLine = QString("%1:%2:%3:%4:%5:%6:131.978:0:0:%7\n")
                .arg(id)
                .arg(type)
                .arg(lat).arg(lon)
                .arg(localX).arg(localY)
                .arg(name);
        _value.append((dataLine));
    }
}
