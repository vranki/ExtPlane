#include "dataref.h"
#include "../util/console.h"

DataRef::DataRef(QObject *parent, const QString &name, void *ref) : QObject(parent)
  , _typeString("?")
  , _type(extplaneRefTypeUnknown)
  , _ref(ref)
  , _accuracy(0)
  , _valueValid(false)
  , _name(name)
  , _subscriberCount(0)
  , _writable(false)
  , _unsubscribeAfterChange(false)
{
    if(name.contains(":")) {
        QString modifiersPart = name.right(name.length() - name.indexOf(":") - 1);
        _modifiers = modifiersPart.split(",");
    }
}

const QString &DataRef::name() const {
    return _name;
}

QStringList DataRef::modifiers() const {
    return _modifiers;
}

void *DataRef::ref() const {
    return _ref;
}

int DataRef::subscriberCount() const {
    return _subscriberCount;
}

void DataRef::setSubscriberCount(const int subs) {
    Q_ASSERT(subs >= 0);
    _subscriberCount = subs;
}

void DataRef::setWritable(const bool cw) {
    _writable = cw;
}

bool DataRef::isWritable() const {
    return _writable;
}

extplaneRefID DataRef::type() const {
    return _type;
}

void DataRef::setType(extplaneRefID newType)
{
    _type = newType;
}

QString DataRef::typeString() const {
    return _typeString;
}

void DataRef::setAccuracy(double val) {
    _accuracy = val;
}

void DataRef::updateAccuracy(double val) {
    // Only update if the new accuracy is higher (eg the value is smaller than currently set)
    if(val < _accuracy) _accuracy = val;
}

void DataRef::setUnsubscribeAfterChange() {
    _unsubscribeAfterChange = true;
}

bool DataRef::shouldUnsubscribeAfterChange() const {
    return _unsubscribeAfterChange;
}

bool DataRef::isValid() const
{
    return _valueValid;
}

void DataRef::setValueValid()
{
    if(!_valueValid) {
        _valueValid = true;
    }
}
