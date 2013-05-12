#include "extplaneclient.h"
#include "../../util/console.h"

ExtPlaneClient::ExtPlaneClient(QObject *parent, QString name, ClientDataRefProvider *drp) :
    QObject(parent), _name(name), _connection(drp)
{
}

ExtPlaneClient::~ExtPlaneClient() {
    foreach(ClientDataRef *ref, _dataRefs) {
        _dataRefs.removeOne(ref);
        _connection->unsubscribeDataRef(ref);
    }
}

ClientDataRef* ExtPlaneClient::subscribeDataRef(QString name, double accuracy) {
    ClientDataRef *ref = _connection->subscribeDataRef(name, accuracy);
    connect(ref, SIGNAL(changed(ClientDataRef*)), this, SLOT(cdrChanged(ClientDataRef*)));
    connect(ref, SIGNAL(destroyed(QObject*)), this, SLOT(refDestroyed(QObject*)));
    _dataRefs.append(ref);
    return ref;
}

void ExtPlaneClient::refDestroyed(QObject* refqo) {
    _dataRefs.removeOne(static_cast<ClientDataRef*>(refqo));
}

void ExtPlaneClient::cdrChanged(ClientDataRef *ref) {
    double value;
    bool ok;
    
    value = ref->valueString().toDouble(&ok);
    if (ok){
        emit refChanged(ref->name(), value);
    } else {
        DEBUG << "unable to convert to double " << ref->valueString();
        emit refChanged(ref->name(), ref->valueString());
    }
}

void ExtPlaneClient::unsubscribeDataRef(QString name) {
    DEBUG << name;
    foreach(ClientDataRef *ref, _dataRefs) {
        if(ref->name() == name) {
            _dataRefs.removeOne(ref);
            _connection->unsubscribeDataRef(ref);
            return;
        }
    }
    Q_ASSERT(false);
}

void ExtPlaneClient::keyPress(int id) {
    _connection->keyPress(id);
}

void ExtPlaneClient::buttonPress(int id) {
    _heldButtons.insert(id);
    _connection->buttonPress(id);
}

void ExtPlaneClient::buttonRelease(int id) {
    if(!_heldButtons.contains(id)) return;
    _heldButtons.remove(id);
    _connection->buttonRelease(id);
}

void ExtPlaneClient::setUpdateInterval(double newInterval) {}

void ExtPlaneClient::valueSet(ClientDataRef *ref) {
    _connection->setValue(ref->name(), ref->valueString());
}

void ExtPlaneClient::unsubscribed(ClientDataRef *ref) {
    unsubscribeDataRef(ref->name());
}
