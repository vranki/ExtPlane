#include "clientdataref.h"
#include "extplaneclient.h"

ClientDataRef::ClientDataRef(QObject *parent, QString newName, double accuracy) : QObject(parent)
  , m_accuracy(accuracy)
{
    setName(newName);
}

ClientDataRef::~ClientDataRef() {
    if(m_subscribers > 0) {
        qWarning() << Q_FUNC_INFO << "Warning: ref " << m_name << " destroyed but still subscribed by " << m_subscribers;
    }
}

QString& ClientDataRef::name() {
    return m_name;
}

void ClientDataRef::setName(QString &name) {
    if (m_name == name)
        return;

    m_name = name;
    if(m_name.contains(":")) {
        QString modifiersPart = name.right(name.length() - name.indexOf(":") - 1);
        m_modifiers = modifiersPart.split(",");
    }
    emit nameChanged(m_name);
}

QString ClientDataRef::value() const {
    return m_values.isEmpty() ? "" : m_values.first();
}

int ClientDataRef::valueInt()
{
    if(!m_valueIntValid) {
        m_valueInt = valueFloat(); // Allows decimals to be converted to ints
        m_valueIntValid = true;
    }
    return m_valueInt;
}

float ClientDataRef::valueFloat() {
    if(std::isnan(m_valueFloat))
        m_valueFloat = value().toFloat();
    return m_valueFloat;
}

double ClientDataRef::valueDouble()
{
    if(std::isnan(m_valueDouble))
        m_valueDouble = value().toDouble();
    return m_valueDouble;
}

ExtPlaneClient *ClientDataRef::client() const {
    return m_client;
}

void ClientDataRef::setClient(ExtPlaneClient *client) {
    if (m_client == client)
        return;
    m_client = client;
    if(m_client) {
        connect(m_client, &ExtPlaneClient::destroyed, this, &ClientDataRef::clientDestroyed);
    }
    invalidateValue();
    emit clientChanged(m_client);
}

void ClientDataRef::clientDestroyed() {
    setClient(nullptr);
}

void ClientDataRef::invalidateValue()
{
    // Invalidate value types
    m_valueIntValid = false;
    m_valueFloat = std::nanf("");
    m_valueDouble = std::nan("");
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

    invalidateValue();

    emit changed(this);
}

void ClientDataRef::updateValue(QStringList &newValues) {
    if(m_changedOnce && newValues == m_values) return;
    m_values = newValues;
    m_changedOnce = true;
    invalidateValue();
    emit changed(this);
}

void ClientDataRef::updateValue(int newValue)
{
    invalidateValue();
    m_valueInt = newValue;
    m_valueIntValid = true;
}

void ClientDataRef::updateValue(float newValue)
{
    invalidateValue();
    m_valueFloat = newValue;
}

void ClientDataRef::updateValue(double newValue)
{
    invalidateValue();
    m_valueDouble = newValue;
}

void ClientDataRef::setValue(QString _newValue, int index) {
    while(m_values.size() < index + 1) // Resize list if needed
        m_values.append(QString(""));
    m_values[index] = _newValue;
    invalidateValue();
    emit valueSet(this);
}

void ClientDataRef::setValues(QStringList values)
{
    if(values == m_values) return;
    m_values = values;
    invalidateValue();
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
    Q_ASSERT(sub >= 0);
    m_subscribers = sub;
}

void ClientDataRef::unsubscribe() {
    emit unsubscribed(this);
}

const QString& ClientDataRef::dataFormat() const {
    return m_dataFormat;
}

const QStringList &ClientDataRef::modifiers() const
{
    return m_modifiers;
}

void ClientDataRef::setUdpId(quint16 id)
{
    m_udpId = id;
}

quint16 ClientDataRef::udpId()
{
    return m_udpId;
}

bool ClientDataRef::isArray() {
    return m_values.size() > 1;
}
