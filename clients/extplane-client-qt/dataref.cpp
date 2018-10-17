#include "dataref.h"
#include <clientdataref.h>
#include <extplaneclient.h>
#include <QQmlApplicationEngine>

DataRef::DataRef(QObject *parent) : QObject(parent)
  , m_clientDataRef(nullptr)
  , m_client(nullptr)
{
    setClient(&ExtPlaneClient::instance());
}

void DataRef::subscribeIfPossible()
{
    if(m_clientDataRef)
        return;

    if(m_name.isEmpty())
        return;

    if(m_client && !m_name.isEmpty()) {
        m_clientDataRef = m_client->subscribeDataRef(m_name, m_accuracy);
        if(!m_clientDataRef) {
            qDebug() << Q_FUNC_INFO << "Unable to subscribe to " << m_name;
        } else {
            connect(m_clientDataRef, &ClientDataRef::changed, this, &DataRef::changed);
            connect(m_clientDataRef, &ClientDataRef::valueSet, this, &DataRef::valueSet);
            connect(m_clientDataRef, &ClientDataRef::unsubscribed, this, &DataRef::unsubscribed);
            connect(m_clientDataRef, &ClientDataRef::nameChanged, this, &DataRef::nameChanged);
            connect(m_clientDataRef, &ClientDataRef::accuracyChanged, this, &DataRef::accuracyChanged);
        }
    }
}

QString& DataRef::name() {
    return m_name;
}

void DataRef::setName(QString &name)
{
    if (m_name == name)
        return;

    m_name = name;
    emit nameChanged(m_name);
    subscribeIfPossible();
}

QString DataRef::value()
{
    return m_clientDataRef ? m_clientDataRef->value() : "";
}

ExtPlaneClient *DataRef::client() const {
    return m_client;
}

void DataRef::setClient(ExtPlaneClient *client)
{
    if (m_client == client)
        return;

    disconnect(client, 0, this, 0);
    m_client = client;
    emit clientChanged(m_client);
    connect(client, &ExtPlaneClient::datarefProviderChanged, this, &DataRef::setDataRefProvider);
    subscribeIfPossible();
}

void DataRef::setDataRefProvider() {
    m_clientDataRef = nullptr;
    if(client()->datarefProvider()) subscribeIfPossible();
}

QStringList& DataRef::values() {
    return m_clientDataRef ? m_clientDataRef->values() : m_emptyStringList;
}

void DataRef::setValue(QString _newValue, int index) {
    if(m_clientDataRef) m_clientDataRef->setValue(_newValue, index);
}

void DataRef::setValues(QStringList values)
{
    if(m_clientDataRef) m_clientDataRef->setValues(values);
}

double DataRef::accuracy() {
    return m_clientDataRef ? m_clientDataRef->accuracy() : m_accuracy;
}

void DataRef::setAccuracy(double accuracy)
{
    m_accuracy = accuracy;
    if(m_clientDataRef) m_clientDataRef->setAccuracy(accuracy);
}
