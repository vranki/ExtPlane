#include "extplanetransformer.h"
#include "datasources/flightgeardatasource.h"
#include <datarefprovider.h>

ExtplaneTransformer::ExtplaneTransformer() :
    QObject()
  , m_dataSource(nullptr)
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

QString ExtplaneTransformer::dataSourceName() const
{
    return m_dataSourceName;
}

QString ExtplaneTransformer::networkError() const
{
    return m_networkError;
}

QObject *ExtplaneTransformer::dataSource() const
{
    return m_dataSource;
}

void ExtplaneTransformer::setDataSourceName(const QString dataSource)
{
    if (m_dataSourceName == dataSource)
        return;

    if(m_dataSource) {
        _server.setDataRefProvider(nullptr);
        delete m_dataSource;
        m_dataSource = nullptr;
    }

    m_dataSourceName = dataSource;
    if(m_dataSourceName == m_dataSources.first()) {
        m_dataSource = nullptr;
    } else {
        m_dataSource = new FlightGearDataSource();
        connect(m_dataSource, &DataSource::sourceNetworkError,
                this, &ExtplaneTransformer::sourceNetworkErrorChanged);
    }
    _server.setDataRefProvider(m_dataSource);
    if(m_dataSource) m_dataSource->connectToSource();

    emit dataSourceChanged();
}

void ExtplaneTransformer::sourceNetworkErrorChanged(QString errorString)
{
    m_networkError = errorString;
    emit networkErrorChanged(errorString);
}
