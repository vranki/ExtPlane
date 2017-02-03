#include "extplanetransformer.h"
#include "datasources/flightgeardatasource.h"
#include <datarefprovider.h>

ExtplaneTransformer::ExtplaneTransformer() : QObject(), m_dataSource(0)
{
    m_dataSources << "None" << "FlightGear";
    m_dataSourceName = m_dataSources.first(); // None
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
    return m_dataSourceName;
}

QString ExtplaneTransformer::networkError() const
{
    return m_networkError;
}

void ExtplaneTransformer::setDataSource(QString dataSource)
{
    if (m_dataSourceName == dataSource)
        return;

    if(m_dataSource) {
        _server.setDataRefProvider(0);
        delete m_dataSource;
        m_dataSource = 0;
    }

    m_dataSourceName = dataSource;
    if(m_dataSourceName == m_dataSources.first()) {
        m_dataSource = 0;
    } else {
        m_dataSource = new FlightGearDataSource();
        connect(m_dataSource, &DataSource::sourceNetworkError,
                this, &ExtplaneTransformer::sourceNetworkErrorChanged);
    }
    _server.setDataRefProvider(m_dataSource);
    if(m_dataSource) m_dataSource->connectToSource();

    emit dataSourceChanged(dataSource);
}

void ExtplaneTransformer::sourceNetworkErrorChanged(QString errorString)
{
    m_networkError = errorString;
    emit networkErrorChanged(errorString);
}
