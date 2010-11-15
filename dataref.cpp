#include "dataref.h"

DataRef::DataRef(QObject *parent, QString name, XPLMDataRef ref) : QObject(parent), _name(name), _ref(ref), _value(-9999999999.0f), _subscribers(0) {
}

QString &DataRef::name() {
    return _name;
}

XPLMDataRef DataRef::ref() {
    return _ref;
}

double DataRef::value() {
    return _value;
}

void DataRef::setValue(double newValue) {
    if(_value != newValue) {
        _value = newValue;
        emit changed(this);
    }
}

int DataRef::subscribers() {
    return _subscribers;
}

void DataRef::setSubscribers(int subs) {
    Q_ASSERT(subs >= 0);
    _subscribers = subs;
}
