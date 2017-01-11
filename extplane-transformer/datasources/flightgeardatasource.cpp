#include "flightgeardatasource.h"
#include <QDebug>

FlightGearDataSource::FlightGearDataSource() : QObject(), DataRefProvider()
{
    qDebug() << Q_FUNC_INFO;
    connect(&tcpSocket, &QTcpSocket::connected,
            this, &FlightGearDataSource::sessionOpened);
    connect(&tcpSocket, &QTcpSocket::readyRead,
            this, &FlightGearDataSource::readReply);
    tcpSocket.connectToHost("localhost", 52000);
}

DataRef *FlightGearDataSource::subscribeRef(QString name)
{
    qDebug() << Q_FUNC_INFO << name;
    return 0;
}

void FlightGearDataSource::unsubscribeRef(DataRef *ref)
{
}

void FlightGearDataSource::updateDataRef(DataRef *ref)
{
}

void FlightGearDataSource::changeDataRef(DataRef *ref)
{
}

void FlightGearDataSource::keyStroke(int keyid)
{
}

void FlightGearDataSource::buttonPress(int buttonid)
{
}

void FlightGearDataSource::buttonRelease(int buttonid)
{
}

void FlightGearDataSource::command(QString &name, extplaneCommandType type)
{
}

void FlightGearDataSource::sessionOpened()
{
    qDebug() << Q_FUNC_INFO;
    tcpSocket.write("get /position/altitude-ft\n");
    tcpSocket.flush();
}

void FlightGearDataSource::readReply()
{
    qDebug() << Q_FUNC_INFO << tcpSocket.bytesAvailable();
    QTextStream ts(&tcpSocket);
    qDebug() << Q_FUNC_INFO<< ts.readLine();
}
