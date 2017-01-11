#ifndef FLIGHTGEARDATASOURCE_H
#define FLIGHTGEARDATASOURCE_H

#include <QObject>
#include <datarefprovider.h>
#include <QTcpSocket>

class FlightGearDataSource : public QObject, public DataRefProvider
{
    Q_OBJECT

public:
    FlightGearDataSource();

    // DataRefProvider interface
public:
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
    void readReply();

private:

    QTcpSocket tcpSocket;
};

#endif // FLIGHTGEARDATASOURCE_H
