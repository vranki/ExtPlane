#include "extplanetransformer.h"
#include "datasources/flightgeardatasource.h"
#include <datarefprovider.h>

ExtplaneTransformer::ExtplaneTransformer() : QObject(), m_dataRefProvider(0)
{
    m_dataSources << "None" << "FlightGear";
    m_dataSource = m_dataSources.first(); // None
    emit dataSourcesChanged(m_dataSources);
}

TcpServer *ExtplaneTransformer::tcpServer()
{
    return &_server;
}

QStringList ExtplaneTransformer::dataSources() const
{
    return m_dataSources;
}

QString ExtplaneTransformer::dataSource() const
{
    return m_dataSource;
}

void ExtplaneTransformer::setDataSource(QString dataSource)
{
    if (m_dataSource == dataSource)
        return;
    qDebug() << Q_FUNC_INFO << dataSource;
    if(m_dataRefProvider) {
        _server.setDataRefProvider(0);
        delete m_dataRefProvider;
        m_dataRefProvider = 0;
    }

    m_dataSource = dataSource;
    if(m_dataSource == m_dataSources.first()) {
        m_dataRefProvider = 0;
    } else {
        m_dataRefProvider = new FlightGearDataSource();
    }
    _server.setDataRefProvider(m_dataRefProvider);
    emit dataSourceChanged(dataSource);
}
