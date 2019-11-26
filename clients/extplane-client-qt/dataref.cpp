#include "dataref.h"
#include <clientdataref.h>
#include <extplaneclient.h>

DataRef::DataRef(QObject *parent) : QObject(parent)
                                    , m_clientDataRef(nullptr)
                                    , m_client(nullptr)
                                    , m_accuracy(0)
                                    , m_scaleFactor(1)
{
    // Connect both valueset and changed to valuechanged
    connect(this, &DataRef::valueSet, this, &DataRef::valueChanged);
    connect(this, &DataRef::changed, this, &DataRef::valueChanged);

    setClient(&ExtPlaneClient::instance());
}

DataRef::~DataRef() {
    if(m_clientDataRef) {
        m_clientDataRef->unsubscribe();
        m_clientDataRef = nullptr;
    }
}

void DataRef::subscribeIfPossible() {
    if(m_clientDataRef)
        return;

    if(m_name.isEmpty())
        return;

    if(m_client && !m_name.isEmpty()) {
        m_clientDataRef = m_client->subscribeDataRef(m_name, m_accuracy);
        if(!m_clientDataRef) {
            qDebug() << Q_FUNC_INFO << "Unable to subscribe to " << m_name;
        } else {
            m_clientDataRef->setDataFormat(m_dataFormat);
            connect(m_clientDataRef, &ClientDataRef::changed, this, &DataRef::changed);
            connect(m_clientDataRef, &ClientDataRef::valueSet, this, &DataRef::valueSet);
            connect(m_clientDataRef, &ClientDataRef::unsubscribed, this, &DataRef::unsubscribed);
            connect(m_clientDataRef, &ClientDataRef::nameChanged, this, &DataRef::nameChanged);
            connect(m_clientDataRef, &ClientDataRef::accuracyChanged, this, &DataRef::accuracyChanged);
            connect(m_clientDataRef, &ClientDataRef::dataFormatChanged, this, &DataRef::dataFormatChanged);
            connect(m_clientDataRef, &ClientDataRef::destroyed, this, &DataRef::clientDatarefDestroyed);
        }
    }
}

QString& DataRef::name() {
    return m_name;
}

void DataRef::setName(QString &name) {
    if (m_name == name)
        return;

    if(!m_name.isEmpty()) {
        if(m_clientDataRef) { // Unsub old dataref..
            qDebug() << Q_FUNC_INFO << "Unsubbing" << m_clientDataRef->name();
            m_clientDataRef->unsubscribe();
            m_clientDataRef = nullptr;
        } else {
            qDebug() << Q_FUNC_INFO << "No cdr - can't unsub " << m_name << " after subscribing" << name;
        }
    }
    m_name = name;
    emit nameChanged(m_name);

    subscribeIfPossible();
}

QString DataRef::value() {
    if(m_clientDataRef) {
        if(qFuzzyCompare(m_scaleFactor, 1)) {
            return m_clientDataRef->value();
        } else {
            bool ok;
            double refValue = m_clientDataRef->value().toDouble(&ok);
            refValue = refValue * m_scaleFactor;
            if(!ok) {
                if(m_clientDataRef->value().isEmpty()) return "";

                qDebug() << Q_FUNC_INFO << "Warning: Ref " << name() << "scale factor is set, but can't convert value to double: " << m_clientDataRef->value();
                return m_clientDataRef->value();
            }
            return QString::number(refValue);
        }
    } else {
        return "";
    }
}

ExtPlaneClient *DataRef::client() const {
    return m_client;
}

QString DataRef::dataFormat() const {
    return m_clientDataRef ?  m_clientDataRef->dataFormat() : "";
}

double DataRef::scaleFactor() const {
    return m_scaleFactor;
}

void DataRef::setClient(ExtPlaneClient *client) {
    if (m_client == client)
        return;

    disconnect(client, nullptr, this, nullptr);
    m_client = client;
    emit clientChanged(m_client);
    connect(client, &ExtPlaneClient::datarefProviderChanged, this, &DataRef::setDataRefProvider);
    subscribeIfPossible();
}

void DataRef::setDataRefProvider() {
    m_clientDataRef = nullptr;
    if(client()->datarefProvider()) subscribeIfPossible();
}

void DataRef::setDataFormat(QString dataFormat) {
    m_dataFormat = dataFormat;
    if(m_clientDataRef) m_clientDataRef->setDataFormat(m_dataFormat);
}

void DataRef::scaleFactor(double scaleFactor) {
    if (qFuzzyCompare(m_scaleFactor, scaleFactor))
        return;

    m_scaleFactor = scaleFactor;
    emit scaleFactorChanged(m_scaleFactor);
    emit valueChanged(m_clientDataRef);
}

void DataRef::clientDatarefDestroyed() {
    m_clientDataRef = nullptr;
}

QStringList& DataRef::values() {
    return m_clientDataRef ? m_clientDataRef->values() : m_emptyStringList;
}

void DataRef::setValue(QString _newValue, int index) {
    if(m_clientDataRef) m_clientDataRef->setValue(_newValue, index);
}

void DataRef::setValues(QStringList values) {
    if(m_clientDataRef) m_clientDataRef->setValues(values);
}

double DataRef::accuracy() {
    return m_clientDataRef ? m_clientDataRef->accuracy() : m_accuracy;
}

void DataRef::setAccuracy(double accuracy) {
    m_accuracy = accuracy;
    if(m_clientDataRef) m_clientDataRef->setAccuracy(accuracy);
}
