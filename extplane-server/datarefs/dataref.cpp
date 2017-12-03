#include "dataref.h"

DataRef::DataRef(QObject *parent, QString name, void *ref) : QObject(parent),
    _ref(ref),
    _name(name),
    _subscribers(0),
    _writable(false),
    _type(extplaneRefTypeUnknown),
_typeString("?")
{
}

QString &DataRef::name() {
    return _name;
}

void *DataRef::ref() {
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

extplaneRefID DataRef::type() {
    return _type;
}

void DataRef::setType(extplaneRefID newType)
{
    _type = newType;
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
