#include "datadataref.h"
#include "../util/console.h"

DataDataRef::DataDataRef(QObject *parent, QString &name, void *ref) : DataRef(parent, name, ref)
{
    _typeString = "b";
    _type = extplaneRefTypeData;
    _lastUpdate.restart();
}

QByteArray &DataDataRef::value() {
    return _value;
}

QByteArray &DataDataRef::newValue()
{
    return _newValue;
}

void DataDataRef::updateValue() {
    // Make sure it's time to update again based on the accuracy
    if(this->accuracy() == 0 || _lastUpdate.elapsed() > this->accuracy()) {
        // TODO: do we really want to make this comparison for large data datarefs? Probably as it's still cheaper than sending over the wire the new data
        if (_newValue != _value) {
            _value = _newValue;
            if(!_valueValid) setValueValid();
            emit changed(this);
        }
    }
}

void DataDataRef::setValue(QByteArray &newValue) {
    //TODO: @dankrusi: finish this implementation and test
    qFatal("Writing of Data DataRefs is not yet supported");
    /*
    // Limit number of values to write to ref length or number of given values
    int numberOfValuesToWrite = qMin(_length, values.size());

    // Convert values to float and copy to _valueArray
    for(int i=0;i<numberOfValuesToWrite;i++) {
        bool ok = true;
        float value = values[i].toFloat(&ok);
        if(!ok) {
            qDebug() << Q_FUNC_INFO << "Invalid value " << values[i] << "in array";
            return;
        }
        _valueArray[i]=value;
    }
    XPLMSetDatavf(_ref, _valueArray, 0, numberOfValuesToWrite);
    */
}

QString DataDataRef::valueString() {
    if(modifiers().contains("string")) {
        return  QString("\"%1\"").arg(QString(_value));
    } else {
        return QString(_value).toUtf8().toBase64();
    }
}

void DataDataRef::setValue(QString &newValue) {
    QByteArray valueBA = newValue.toUtf8();
    setValue(valueBA);
}

void DataDataRef::setLength(int newLength)
{
    Q_ASSERT(newLength > 0);
    if(_value.length() != newLength) {
        _value = QByteArray(newLength, 0);
        _newValue = QByteArray(newLength, 0); // Init already here for perf reasons.
    }
}
