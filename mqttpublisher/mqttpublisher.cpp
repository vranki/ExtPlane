#include "mqttpublisher.h"
#include "mqttclient.h"
#include "../extplane-server/datarefs/dataref.h"
#include "../extplane-server/datarefs/floatdataref.h"
#include <mosquittopp.h>
#include <QDebug>

#define CLIENT_ID "ExtPlane-Plugin"
#define BROKER_ADDRESS "localhost"
#define MQTT_PORT 1883;

MQTTPublisher::MQTTPublisher(DataRefProvider *provider, QObject *parent) : QObject(parent)
  , m_refProvider(provider) {
    if(!m_refProvider) qWarning() << Q_FUNC_INFO << "No datarefprovider!";
}

void MQTTPublisher::start()
{
    if(m_mqttClient) return;

    char client_id[] = CLIENT_ID;
    char host[] = BROKER_ADDRESS;
    int port = MQTT_PORT;

    m_connected = false;
    mosqpp::lib_init();

    m_mqttClient = new MQTTClient(client_id, host, port);
    connect(m_mqttClient, &MQTTClient::subscriptionChanged, this, &MQTTPublisher::subscriptionChanged);
    connect(m_mqttClient, &MQTTClient::connectedChanged, this, &MQTTPublisher::connectedChanged);
    connect(m_mqttClient, &MQTTClient::subscriptionRequested, this, &MQTTPublisher::subscriptionRequested);
    connect(m_mqttClient, &MQTTClient::setRef, this, &MQTTPublisher::setRef);
}

void MQTTPublisher::poll()
{
    int rc = m_mqttClient->loop(0);
    if (rc) {
        qDebug() << Q_FUNC_INFO << "Not connected, asking reconnect..";
        m_connected = false;
        m_mqttClient->reconnect();
    }
}

void MQTTPublisher::connectedChanged(bool connected)
{
    qDebug() << Q_FUNC_INFO << connected;
    m_connected = connected;
    if(m_connected) { // Publish all valid values
        for(auto ref : m_refs) {
            if(ref->isValid()) {
                refChanged(ref);
            }
        }
        // m_mqttClient->subscribe(NULL, "ExtPlane/mgmt/#", 2);
        m_mqttClient->subscribe(NULL, "ExtPlane/subscribe", 2);
        m_mqttClient->subscribe(NULL, "ExtPlane/set/#", 2);
    }
}

void MQTTPublisher::subscriptionChanged(bool subscribed)
{
    qDebug() << Q_FUNC_INFO << subscribed;
    m_subscribed = subscribed;
}

void MQTTPublisher::refChanged(DataRef *ref) {
    if(m_connected) {
        QString refTopic = "ExtPlane/" + ref->name();
        if(ref->dataSize() > 0) {
            m_mqttClient->publish(NULL, qPrintable(refTopic), ref->dataSize(), ref->rawData(), 1, true);
        }
    }
}

void MQTTPublisher::subscriptionRequested(QString &refName)
{
    if(m_refProvider) {
        for(auto ref : m_refs) {
            if(ref->name() == refName) return; // Already subbed
        }
        subscribeTo(refName);
    }
}

void MQTTPublisher::setRef(QString &refName, void *data, size_t dataSize)
{
    for(auto ref : m_refs) {
        if(ref->name() == refName) {
            if(ref->dataSize() == dataSize) {
                ref->fromRawData(data);
                m_refProvider->changeDataRef(ref);
                qDebug() << Q_FUNC_INFO << refName << dataSize << ref->valueString();
                return;
            } else {
                qWarning() << Q_FUNC_INFO << "Data size mismatch for" << refName << dataSize << "!=" << ref->dataSize();
            }
        }
    }
    if(subscribeTo(refName)) {
        setRef(refName, data, dataSize);
    } else {
        qWarning() << Q_FUNC_INFO << "Ref" << refName << "not known!";
    }
}

bool MQTTPublisher::subscribeTo(QString &refName)
{
    auto ref = m_refProvider->subscribeRef(refName);
    if(ref) {
        connect(ref, &DataRef::changed, this, &MQTTPublisher::refChanged);
        m_refs.push_back(ref);
    } else {
        qWarning() << Q_FUNC_INFO << "Ref not found" << refName;
    }
    return ref;
}

void MQTTPublisher::stop()
{
    m_connected = false;
    if(m_refProvider) {
        for(auto ref : m_refs) {
            m_refProvider->unsubscribeRef(ref);
        }
    }
    m_refs.clear();

    if(m_mqttClient) {
        delete m_mqttClient;
        m_mqttClient = nullptr;
        mosqpp::lib_cleanup();
    }
}
