#include "doubledataref.h"

DoubleDataRef::DoubleDataRef(QObject *parent, QString name, XPLMDataRef ref) : DataRef(parent, name, ref), _value(-999999.0f) {
    _typeString = "d";
    _type = xplmType_Double;
}

double DoubleDataRef::value() {
    return _value;
}

void DoubleDataRef::updateValue() {
   double newValue = XPLMGetDatad(_ref);
    if(_value != newValue) {
        _value = newValue;
        emit changed(this);
    }
}

void DoubleDataRef::setValue(double newValue) {
    if(!isWritable()) {
        qDebug() << Q_FUNC_INFO << "Tried to write read-only dataref" << name();
        return;
    }
    qDebug() << Q_FUNC_INFO << name() << newValue;
    _value = newValue;
    XPLMSetDataf(ref(), _value);
}

QString DoubleDataRef::valueString() {
    return QString::number(value(), 'g', 10);
}

void DoubleDataRef::setValue(QString &newValue) {
    bool ok = false;
    double value = newValue.toDouble(&ok);
    if(ok) {
        setValue(value);
    } else {
        qDebug() << Q_FUNC_INFO << "Cannot set value " << newValue;
    }
}
