#include "intdataref.h"
#include "../util/console.h"

IntDataRef::IntDataRef(QObject *parent, QString name, void *ref) : DataRef(parent, name, ref), _value(-999999)
{
    _typeString = "i";
    _type = extplaneRefTypeInt;
}

int IntDataRef::value() {
    return _value;
}

void IntDataRef::updateValue(int newValue) {
    if(_value != newValue || !isValid()) {
        _value = newValue;
        if(!_valueValid) setValueValid();
        emit changed(this);
    }
}

QString IntDataRef::valueString() {
    return QString::number(value());
}

void IntDataRef::setValue(QString &newValue) {
    bool ok = false;
    int value = newValue.toInt(&ok);
    if(ok) {
        updateValue(value);
    } else {
        INFO << "Cannot set value " << newValue;
    }
}

size_t IntDataRef::dataSize()
{
    return 4;
}

void *IntDataRef::rawData()
{
    return static_cast<void*>(&_value);
}

void IntDataRef::fromRawData(void *ptr)
{
    int newValue = *static_cast<int*>(ptr);
    updateValue(newValue);
}
