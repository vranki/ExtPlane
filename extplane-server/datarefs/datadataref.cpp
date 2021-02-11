#include "datadataref.h"
#include "../../util/console.h"

DataDataRef::DataDataRef(QObject *parent, QString &name, void *ref)
    : DataRef(parent, name, ref)
{
    _typeString = "b";
    _type = extplaneRefTypeData;
    m_lastUpdate.restart();
}

QByteArray &DataDataRef::value() {
    return m_value;
}

QByteArray &DataDataRef::newValue() {
    return m_newValue;
}

void DataDataRef::updateValue() {
    // Make sure it's time to update again based on the accuracy
    if(this->accuracy() == 0 || (m_lastUpdate.elapsed() > this->accuracy() ) ) {
        // TODO: do we really want to make this comparison for large data datarefs? Probably as it's still cheaper than sending over the wire the new data
        if (m_newValue != m_value || !isValid()) {
            m_value = m_newValue;
            if(!_valueValid) setValueValid();
            emit changed(this);
            m_lastUpdate.restart();
        }
    }
}

void DataDataRef::setValue(QByteArray &newValue) {
    // qDebug() << Q_FUNC_INFO << "Set ref" <<  << "to string" << s << "length" << length;
}

QString DataDataRef::valueString() {
    if(modifiers().contains("string")) { // Add quotes to strings
        return  QString("\"%1\"").arg(QString(m_value));
    } else {
        return m_value.toBase64();
    }
}

void DataDataRef::setValue(QString &newValue) {
    m_value = newValue.toUtf8();
    setValueValid();
    emit changed(this);
    // setValue(valueBA);
}

void DataDataRef::setLength(int newLength) {
    Q_ASSERT(newLength >= 0);
    if(m_value.length() != newLength) {
        m_value = QByteArray(newLength, 0);
        m_newValue = QByteArray(newLength, 0); // Init already here for perf reasons.
    }
}
