#ifndef MQTTCLIENT_H
#define MQTTCLIENT_H

#include <QObject>
#include <mosquittopp.h>

// Credit to https://github.com/Auctoris/mosquitopp_client/blob/master/mqtt.h

#define MAX_PAYLOAD 50
#define DEFAULT_KEEP_ALIVE 60


class MQTTClient : public QObject, public mosqpp::mosquittopp
{
    Q_OBJECT
public:
    explicit MQTTClient(const char *id, const char *host, int port, QObject *parent = nullptr);
    ~MQTTClient();

    virtual void on_connect(int rc);
    virtual void on_message(const struct mosquitto_message *message);
    virtual void on_subscribe(int mid, int qos_count, const int *granted_qos);
signals:
    void connectedChanged(bool connected);
    void subscriptionChanged(bool subscribed);
    void subscriptionRequested(QString &refName);
    void setRef(QString &refName, void *data, size_t dataSize);
};

#endif // MQTTCLIENT_H
