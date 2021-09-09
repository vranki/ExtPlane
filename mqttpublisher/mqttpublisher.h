#ifndef MQTTPUBLISHER_H
#define MQTTPUBLISHER_H

#include <QObject>
#include <QStringList>
#include "../extplane-server/datarefprovider.h"

class MQTTClient;


class MQTTPublisher : public QObject
{
    Q_OBJECT
public:
    explicit MQTTPublisher(DataRefProvider *provider, QObject *parent = nullptr);

public slots:
    void start();
    void stop();
    void poll();

private slots:
    void connectedChanged(bool connected);
    void subscriptionChanged(bool subscribed);
    void refChanged(DataRef *ref);
    void subscriptionRequested(QString &refName);
    void setRef(QString &refName, void *data, size_t dataSize);
    bool subscribeTo(QString &refName);
private:
    MQTTClient *m_mqttClient = nullptr;
    DataRefProvider *m_refProvider = nullptr;
    bool m_subscribed = false;
    bool m_connected = false;
    std::vector<DataRef*> m_refs;
};

#endif // MQTTPUBLISHER_H
