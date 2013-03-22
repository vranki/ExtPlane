#include "clientdataref.h"

ClientDataRef::ClientDataRef(QObject *parent, QString newName, double accuracy) : QObject(parent), _name(newName), _accuracy(accuracy) {
    _subscribers = 0;
    _values.reserve(1); // Always at least 1 size
}

QString& ClientDataRef::name() {
    return _name;
}

QString& ClientDataRef::valueString() {
    return _values.first();
}

QStringList& ClientDataRef::valueStrings() {
    return _values;
}

void ClientDataRef::updateValue(QString newValue) {
    if(!_values.isEmpty() && newValue == _values.first()) return;

    if(_values.isEmpty()) {
        _values.push_back(newValue);
    } else {
        _values.replace(0, newValue);
    }
    emit changed(this);
}

void ClientDataRef::updateValue(QStringList &newValues) {
    if(newValues == _values) return;
    _values = newValues;
    emit changed(this);
}

double ClientDataRef::accuracy() {
    return _accuracy;
}

int ClientDataRef::subscribers() {
    return _subscribers;
}

void ClientDataRef::setSubscribers(int sub) {
    _subscribers = sub;
}

void ClientDataRef::setValue(double _newValue, int index) {
    if(_values.size() < index + 1)
        _values.reserve(index+1);
    _values[index] = QString::number(_newValue);
    emit valueSet(this);
}

void ClientDataRef::unsubscribe() {
    emit unsubscribed(this);
}

bool ClientDataRef::isArray() {
    return _values.size() > 1;
}
