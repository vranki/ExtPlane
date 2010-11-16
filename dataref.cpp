#include "dataref.h"

DataRef::DataRef(QObject *parent, QString name, XPLMDataRef ref) : QObject(parent), _name(name), _ref(ref), _subscribers(0), _writable(false) {
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

void DataRef::setCanWrite(bool cw) {
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
