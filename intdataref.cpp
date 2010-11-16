#include "intdataref.h"

IntDataRef::IntDataRef(QObject *parent, QString name, XPLMDataRef ref) : DataRef(parent, name, ref), _value(-999999)
{
    _typeString = "i";
    _type = xplmType_Int;
}

int IntDataRef::value() {
    return _value;
}

void IntDataRef::updateValue() {
   int newValue = XPLMGetDatai(ref());
    if(_value != newValue) {
        _value = newValue;
        emit changed(this);
    }
}

void IntDataRef::setValue(int newValue) {
    if(!isWritable()) {
        qDebug() << Q_FUNC_INFO << "Tried to write read-only dataref" << name();
        return;
    }
    qDebug() << Q_FUNC_INFO << name() << newValue;
    _value = newValue;
    XPLMSetDatai(ref(), _value);
}

QString IntDataRef::valueString() {
    return QString::number(value());
}

void IntDataRef::setValue(QString &newValue) {
    bool ok = false;
    int value = newValue.toInt(&ok);
    if(ok) {
        setValue(value);
    } else {
        qDebug() << Q_FUNC_INFO << "Cannot set value " << newValue;
    }
}
