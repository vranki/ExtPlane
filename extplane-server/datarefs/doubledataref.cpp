#include "doubledataref.h"
#include "../util/console.h"

DoubleDataRef::DoubleDataRef(QObject *parent, QString name, void *ref) : DataRef(parent, name, ref), _value(-999999.0f) {
    _typeString = "d";
    _type = extplaneRefTypeDouble;
}

double DoubleDataRef::value() {
    return _value;
}

void DoubleDataRef::updateValue(double newValue) {
    if(_value != newValue) {
        _value = newValue;
        if(!_valueValid) setValueValid();
        emit changed(this);
    }
}

QString DoubleDataRef::valueString() {
    return QString::number(value(), 'g', 10);
}

void DoubleDataRef::setValue(QString &newValue) {
    bool ok = false;
    double value = newValue.toDouble(&ok);
    if(ok) {
        updateValue(value);
    } else {
        INFO << "Cannot set value " << newValue;
    }
}
