#include "clientdataref.h"
#include "extplaneclient.h"

ClientDataRef::ClientDataRef(QObject *parent, QString newName, double accuracy) : QObject(parent)
                                                                                  , m_name(newName)
                                                                                  , m_accuracy(accuracy)
                                                                                  , m_subscribers(0)
                                                                                  , m_client(nullptr)
                                                                                  , m_changedOnce(false)
{
    qDebug() << Q_FUNC_INFO << m_name;
}

ClientDataRef::~ClientDataRef() {
    if(m_subscribers > 0) {
        qDebug() << Q_FUNC_INFO << "Warning: ref " << m_name << " destroyed but still subscribed by " << m_subscribers;
    }
}

QString& ClientDataRef::name() {
    return m_name;
}

void ClientDataRef::setName(QString &name) {
    if (m_name == name)
        return;

    m_name = name;
    emit nameChanged(m_name);
}

QString ClientDataRef::value() {
    return m_values.isEmpty() ? "" : m_values.first();
}

ExtPlaneClient *ClientDataRef::client() const {
    return m_client;
}

void ClientDataRef::setClient(ExtPlaneClient *client) {
    if (m_client == client)
        return;
    qDebug() << Q_FUNC_INFO << name() << client;
    m_client = client;
    if(m_client) {
        connect(m_client, &ExtPlaneClient::destroyed, this, &ClientDataRef::clientDestroyed);
    }
    emit clientChanged(m_client);
}

void ClientDataRef::clientDestroyed() {
    qDebug() << Q_FUNC_INFO << name();
    setClient(nullptr);
}

void ClientDataRef::setDataFormat(QString dataFormat) {
    if (m_dataFormat == dataFormat)
        return;

    m_dataFormat = dataFormat;
    emit dataFormatChanged(m_dataFormat);
}

QStringList& ClientDataRef::values() {
    return m_values;
}

void ClientDataRef::updateValue(QString newValue) {
    if(m_changedOnce && !m_values.isEmpty() && newValue == m_values.first()) return;

    if(m_values.isEmpty()) {
        m_values.push_back(newValue);
    } else {
        m_values.replace(0, newValue);
    }
    m_changedOnce = true;
    emit changed(this);
}

void ClientDataRef::updateValue(QStringList &newValues) {
    if(m_changedOnce && newValues == m_values) return;
    m_values = newValues;
    m_changedOnce = true;
    emit changed(this);
}

void ClientDataRef::setValue(QString _newValue, int index) {
    while(m_values.size() < index + 1) // Resize list if needed
        m_values.append(QString(""));
    m_values[index] = _newValue;
    emit valueSet(this);
}

void ClientDataRef::setValues(QStringList values)
{
    if(values == m_values) return;
    m_values = values;
    emit valueSet(this);
}

double ClientDataRef::accuracy() {
    return m_accuracy;
}

void ClientDataRef::setAccuracy(double accuracy) {
    // http://doc.qt.io/qt-5/qtglobal.html#qFuzzyCompare
    if (qFuzzyIsNull(accuracy)) {
        if (qFuzzyCompare(1 + m_accuracy, 1 + accuracy))
            return;
    } else {
        if (qFuzzyCompare(m_accuracy, accuracy))
            return;
    }

    m_accuracy = accuracy;
    emit accuracyChanged(m_accuracy);
}

int ClientDataRef::subscribers() {
    return m_subscribers;
}

void ClientDataRef::setSubscribers(int sub) {
    qDebug() << Q_FUNC_INFO << name() << sub;
    Q_ASSERT(sub >= 0);
    m_subscribers = sub;
}

void ClientDataRef::unsubscribe() {
    qDebug() << Q_FUNC_INFO << m_client << m_subscribers;
    emit unsubscribed(this);
}

QString ClientDataRef::dataFormat() const {
    return m_dataFormat;
}

bool ClientDataRef::isArray() {
    return m_values.size() > 1;
}
