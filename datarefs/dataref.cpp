#include "dataref.h"

DataRef::DataRef(QObject *parent, QString name, XPLMDataRef ref) : QObject(parent), _ref(ref), _name(name), _subscribers(0), _writable(false) {
    _type = XPLMGetDataRefTypes(_ref);
    _typeString = "?";
}

QString &DataRef::name() {
    return _name;
}

XPLMDataRef DataRef::ref() {
    return _ref;
}

int DataRef::subscribers() {
    return _subscribers;
}

void DataRef::setSubscribers(int subs) {
    Q_ASSERT(subs >= 0);
    _subscribers = subs;
}

void DataRef::setWritable(bool cw) {
    _writable = cw;
}

bool DataRef::isWritable() {
    return _writable;
}

XPLMDataTypeID DataRef::type() {
    return _type;
}

QString DataRef::typeString() {
    return _typeString;
}

void DataRef::setAccuracy(double val) {
    _accuracy = val;
}

void DataRef::updateAccuracy(double val) {
    // Only update if the new accuracy is higher (eg the value is smaller than currently set)
    if(val < _accuracy) _accuracy = val;
}
