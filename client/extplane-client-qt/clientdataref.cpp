#include "clientdataref.h"

ClientDataRef::ClientDataRef(QObject *parent, QString newName, double accuracy) : QObject(parent), _name(newName), _accuracy(accuracy) {
    _subscribers = 0;
}

QString ClientDataRef::name() {
    return _name;
}

QString ClientDataRef::valueString() {
    return _value;
}

void ClientDataRef::updateValue(QString newValue) {
    if(newValue == _value) return;
    _value = newValue;
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

void ClientDataRef::setValue(double _newValue) {
    _value = QString::number(_newValue);
    emit valueSet(this);
}

void ClientDataRef::unsubscribe() {
    emit unsubscribed(this);
}
