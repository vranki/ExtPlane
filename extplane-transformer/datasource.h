#ifndef DATASOURCE_H
#define DATASOURCE_H

#include <QObject>
#include "datarefprovider.h"

class DataSource : public QObject, public DataRefProvider
{
    Q_OBJECT
public:
    explicit DataSource(QObject *parent = 0);
    virtual void connectToSource(QString host="", int port=0)=0;
signals:
    void sourceNetworkError(QString errorString);
};

#endif // DATASOURCE_H
