#ifndef DATASOURCE_H
#define DATASOURCE_H

#include <QObject>
#include "datarefprovider.h"

class DataSource : public QObject, public DataRefProvider
{
    Q_OBJECT
    Q_PROPERTY(QString helpText READ helpText NOTIFY helpTextChanged)

public:
    explicit DataSource(QObject *parent = 0);
    virtual void connectToSource(QString host="", int port=0)=0;
    QString helpText() const;

signals:
    void sourceNetworkError(QString errorString);
    void helpTextChanged(QString helpText);
protected:
    void setHelpText(QString newText);
private:
    QString m_helpText;
};

#endif // DATASOURCE_H
