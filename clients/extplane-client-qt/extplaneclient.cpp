#include "extplaneclient.h"
#include "../../util/console.h"

ExtPlaneClient::ExtPlaneClient() : QObject()
  , m_name("ExtPlane Qt client")
  , m_connection(nullptr)
{}

ExtPlaneClient::ExtPlaneClient(QObject *parent, QString name, ClientDataRefProvider *drp) : QObject(parent)
  , m_name(name)
  , m_connection(drp)
{}

ExtPlaneClient::~ExtPlaneClient() {
    for(ClientDataRef *ref : m_dataRefs) {
        m_dataRefs.removeOne(ref);
        m_connection->unsubscribeDataRef(ref);
    }
}

ClientDataRef* ExtPlaneClient::subscribeDataRef(QString name, double accuracy) {
    ClientDataRef *ref = m_connection->subscribeDataRef(name, accuracy);
    connect(ref, SIGNAL(changed(ClientDataRef*)), this, SLOT(cdrChanged(ClientDataRef*)));
    connect(ref, SIGNAL(destroyed(QObject*)), this, SLOT(refDestroyed(QObject*)));
    m_dataRefs.append(ref);
    return ref;
}

void ExtPlaneClient::refDestroyed(QObject* refqo) {
    m_dataRefs.removeOne(static_cast<ClientDataRef*>(refqo));
}

void ExtPlaneClient::cdrChanged(ClientDataRef *ref) {
    double value;
    bool ok;
    if(ref->isArray()) {
        emit refChanged(ref->name(), ref->values());
    } else {
        // Try to convert to double and forward to corresponding slot per default
        // If that fails, we fallback to the string slot
        // TODO: Is this really a nice solution?
        value = ref->value().toDouble(&ok);
        if (ok){
            emit refChanged(ref->name(), value);
        } else {
            emit refChanged(ref->name(), ref->value());
        }
    }
}

void ExtPlaneClient::unsubscribeDataRef(QString name) {
    DEBUG << name;
    for(ClientDataRef *ref : m_dataRefs) {
        if(ref->name() == name) {
            m_dataRefs.removeOne(ref);
            m_connection->unsubscribeDataRef(ref);
            return;
        }
    }
    Q_ASSERT(false);
}

bool ExtPlaneClient::isDataRefSubscribed(QString name) {
    for(ClientDataRef *ref : m_dataRefs) {
        if(ref->name() == name) {
            return true;
        }
    }
    return false;
}

void ExtPlaneClient::keyPress(int id) {
    m_connection->keyPress(id);
}

void ExtPlaneClient::buttonPress(int id) {
    m_heldButtons.insert(id);
    m_connection->buttonPress(id);
}

void ExtPlaneClient::buttonRelease(int id) {
    if(!m_heldButtons.contains(id)) return;
    m_heldButtons.remove(id);
    m_connection->buttonRelease(id);
}

void ExtPlaneClient::commandOnce(QString name) {
    m_connection->commandOnce(name);
}

void ExtPlaneClient::commandBegin(QString name) {
    m_runningCommands.insert(name);
    m_connection->commandBegin(name);
}

void ExtPlaneClient::commandEnd(QString name) {
    if(!m_runningCommands.contains(name)) return;
    m_runningCommands.remove(name);
    m_connection->commandEnd(name);
}

ClientDataRefProvider *ExtPlaneClient::datarefProvider() const
{
    return m_connection;
}

void ExtPlaneClient::setUpdateInterval(double newInterval) {
    Q_UNUSED(newInterval);
}

void ExtPlaneClient::setDatarefProvider(ClientDataRefProvider *datarefProvider)
{
    if (m_connection == datarefProvider)
        return;

    m_connection = datarefProvider;
    emit datarefProviderChanged(m_connection);
}

void ExtPlaneClient::valueSet(ClientDataRef *ref) {
    m_connection->setValue(ref->name(), ref->value());
}

void ExtPlaneClient::unsubscribed(ClientDataRef *ref) {
    unsubscribeDataRef(ref->name());
}
