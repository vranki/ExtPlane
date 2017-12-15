#ifndef DATASOURCE_H
#define DATASOURCE_H

#include <QObject>
#include "datarefprovider.h"
/**
 * @brief The DataSource class is the class to subclass for writing
 * new datasources.
 *
 * It inherits from DataRefProvider so you'll need to implement
 * those functions also.
 */
class DataSource : public QObject, public DataRefProvider
{
    Q_OBJECT
    // Human-readable help text to display to user about the state of this datasource
    Q_PROPERTY(QString helpText READ helpText NOTIFY helpTextChanged)

public:
    explicit DataSource(QObject *parent = nullptr);
    // Connect to host & port supplied by user, or localhost & default port if not set.
    virtual void connectToSource()=0;
    QString helpText() const;

signals:
    // Emit this to display an error text on the UI
    void sourceNetworkError(QString errorString);
    void helpTextChanged(QString helpText);

protected:
    // Sets the help text. Use this to change the text.
    void setHelpText(QString newText);
    void setNetworkError(QString newError);
private:
    QString m_helpText, m_networkError;
};

#endif // DATASOURCE_H
