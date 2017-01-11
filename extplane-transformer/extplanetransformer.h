#ifndef EXTPLANETRANSFORMER_H
#define EXTPLANETRANSFORMER_H

#include <QObject>
#include "../extplane-server/tcpserver.h"

class ExtplaneTransformer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject *tcpServer READ tcpServer NOTIFY tcpServerChanged)
    // First is always null
    Q_PROPERTY(QStringList dataSources READ dataSources NOTIFY dataSourcesChanged)
    Q_PROPERTY(QString dataSource READ dataSource WRITE setDataSource NOTIFY dataSourceChanged)

public:
    ExtplaneTransformer();
    TcpServer* tcpServer();
    QStringList dataSources() const;
    QString dataSource() const;

public slots:
    void setDataSource(QString dataSource);

signals:
    void tcpServerChanged(TcpServer* tcpServer);
    void dataSourcesChanged(QStringList dataSources);
    void dataSourceChanged(QString dataSource);

private:
    TcpServer _server;
    TcpServer* m_tcpServer;
    QStringList m_dataSources;
    QString m_dataSource;
    DataRefProvider * m_dataRefProvider;
};

#endif // EXTPLANETRANSFORMER_H
