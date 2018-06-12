#ifndef EXTPLANECLIENT_H
#define EXTPLANECLIENT_H

#include <QObject>
#include <QString>
#include <QList>
#include <QSet>
#include "clientdataref.h"
#include "clientdatarefprovider.h"

/**
 * Represents a single ExtPlane client
 */
class ExtPlaneClient : public QObject {
    Q_OBJECT
    Q_PROPERTY(ClientDataRefProvider* datarefProvider READ datarefProvider WRITE setDatarefProvider NOTIFY datarefProviderChanged)

public:
    explicit ExtPlaneClient();
    explicit ExtPlaneClient(QObject *parent, QString name, ClientDataRefProvider *drp);
    ~ExtPlaneClient();
    ClientDataRef* subscribeDataRef(QString name, double accuracy=0);
    void unsubscribeDataRef(QString name);
    bool isDataRefSubscribed(QString name);
    void keyPress(int id);
    void buttonPress(int id);
    void buttonRelease(int id);
    void commandOnce(QString name);
    void commandBegin(QString name);
    void commandEnd(QString name);
    ClientDataRefProvider* datarefProvider() const;

public slots:
    void setUpdateInterval(double newInterval);
    void setDatarefProvider(ClientDataRefProvider* datarefProvider);

signals:
    void refChanged(QString name, double value);
    void refChanged(QString name, QString value);
    void refChanged(QString name, QStringList values);
    void datarefProviderChanged(ClientDataRefProvider* datarefProvider);

private slots:
    void cdrChanged(ClientDataRef *ref);
    void valueSet(ClientDataRef *ref);
    void unsubscribed(ClientDataRef *ref);
    void refDestroyed(QObject* refqo);

private:
    QString m_name;
    QList<ClientDataRef*> m_dataRefs;
    QSet<int> m_heldButtons;
    QSet<QString> m_runningCommands;
    ClientDataRefProvider *m_connection; // The actual connection class
};

#endif // EXTPLANECLIENT_H
