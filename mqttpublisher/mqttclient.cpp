#include "mqttclient.h"
#include <QDebug>

#define PUBLISH_TOPIC "ExtPlane"

MQTTClient::MQTTClient(const char *id, const char *host, int port, QObject *parent) : QObject(parent)
  , mosqpp::mosquittopp(id)
{
    int keepalive = DEFAULT_KEEP_ALIVE;
    mosqpp::mosquittopp::connect(host, port, keepalive);
}

MQTTClient::~MQTTClient() {}

void MQTTClient::on_connect(int rc)
{
    if (!rc) {
        qDebug() << "Connected - code " << rc;
    }
    emit connectedChanged(!rc);
}

void MQTTClient::on_subscribe(int mid, int qos_count, const int *granted_qos)
{
    qDebug() << "Subscription succeeded." << mid << qos_count << *granted_qos;
    emit subscriptionChanged(true);
}

void MQTTClient::on_message(const struct mosquitto_message *message)
{
    QString topic = message->topic;
    if(topic == "ExtPlane/subscribe") {
        QString refName = QString::fromUtf8(static_cast<char*>(message->payload), message->payloadlen);
        emit subscriptionRequested(refName);
    } else if(topic.startsWith("ExtPlane/set/")) {
        QString refName = topic.remove("ExtPlane/set/");
        emit setRef(refName, message->payload, message->payloadlen);
    }
}
