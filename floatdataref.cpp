#include "floatdataref.h"

FloatDataRef::FloatDataRef(QObject *parent, QString name, XPLMDataRef ref) : DataRef(parent, name, ref), _value(-999999.0f)
{
    _typeString = "f";
    _type = xplmType_Float;
}

float FloatDataRef::value() {
    return _value;
}

void FloatDataRef::updateValue() {
   float newValue = XPLMGetDataf(ref());
    if(_value != newValue) {
        _value = newValue;
        emit changed(this);
    }
}


void FloatDataRef::setValue(float newValue) {
    if(!isWritable()) {
        qDebug() << Q_FUNC_INFO << "Tried to write read-only dataref" << name();
        return;
    }
    qDebug() << Q_FUNC_INFO << name() << newValue;
    _value = newValue;
    XPLMSetDataf(ref(), _value);
}

QString FloatDataRef::valueString() {
    return QString::number(value(), 'g', 10);
}

void FloatDataRef::setValue(QString &newValue) {
    bool ok = false;
    float value = newValue.toFloat(&ok);
    if(ok) {
        setValue(value);
    } else {
        qDebug() << Q_FUNC_INFO << "Cannot set value " << newValue;
    }
}
