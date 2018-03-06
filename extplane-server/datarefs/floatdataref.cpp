#include "floatdataref.h"
#include "../util/console.h"

FloatDataRef::FloatDataRef(QObject *parent, QString name, void *ref) : DataRef(parent, name, ref),
    _value(-999999.0f)
{
    _type = extplaneRefTypeFloat;
    _typeString = "f";
}

float FloatDataRef::value() {
    return _value;
}

void FloatDataRef::updateValue(float newValue) {
    if(_value != newValue) {
        _value = newValue;
        if(!_valueValid) setValueValid();
        emit changed(this);
    }
}

QString FloatDataRef::valueString() {
    return QString::number(value(), 'g', 10);
}

void FloatDataRef::setValue(QString &newValue)
{
    bool ok = false;
    float value = newValue.toFloat(&ok);
    if(ok) {
        updateValue(value);
    } else {
        INFO << "Cannot set value " << newValue;
    }

}
