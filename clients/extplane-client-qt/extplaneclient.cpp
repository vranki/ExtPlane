#include "extplaneclient.h"
#include "../../util/console.h"

ExtPlaneClient* ExtPlaneClient::_instance = nullptr;

ExtPlaneClient::ExtPlaneClient(bool simulated) : QObject()
  , m_name("ExtPlane Qt client")
  , m_connection(nullptr)
  , m_simulated(simulated)
{
    _instance = this;
}

ExtPlaneClient::ExtPlaneClient(QObject *parent, QString name, bool simulated) : QObject(parent)
  , m_name(name)
  , m_connection(nullptr)
  , m_simulated(simulated)
{
    _instance = this;
}

void ExtPlaneClient::createClient() {
    if(m_connection) return; // Already created
    connect(&m_simulatedExtplaneConnection, &ExtPlaneConnection::connectionMessage, this, &ExtPlaneClient::setConnectionMessage);
    connect(&m_simulatedExtplaneConnection, &ExtPlaneConnection::connectedChanged, this, &ExtPlaneClient::connectedChanged);
    connect(&m_extplaneConnection, &ExtPlaneConnection::connectionMessage, this, &ExtPlaneClient::setConnectionMessage);
    connect(&m_extplaneConnection, &ExtPlaneConnection::extplaneWarning, this, &ExtPlaneClient::extplaneWarning);
    connect(&m_extplaneConnection, &ExtPlaneConnection::connectedChanged, this, &ExtPlaneClient::connectedChanged);
    qDebug() << Q_FUNC_INFO << "simulated:" << m_simulated;
    if(m_simulated) {
        m_simulatedExtplaneConnection.registerClient(this);
        m_simulatedExtplaneConnection.startConnection();
    } else {
        m_extplaneConnection.registerClient(this);
        m_extplaneConnection.startConnection();
    }
    m_connection = m_simulated ? &m_simulatedExtplaneConnection : &m_extplaneConnection;
}

ExtPlaneClient::~ExtPlaneClient() {
    while (!m_dataRefs.empty()) {
        auto *ref = *m_dataRefs.begin();
        m_dataRefs.erase(ref);
        if(ref->client()) {
            m_connection->unsubscribeDataRef(ref);
            ref->setClient(nullptr);
        }
    }
}

ExtPlaneClient &ExtPlaneClient::instance()
{
    if (!_instance) _instance = new ExtPlaneClient(true);
    _instance->createClient();
    return *_instance;
}

ClientDataRef* ExtPlaneClient::subscribeDataRef(QString name, double accuracy) {
    Q_ASSERT(m_connection);
    ClientDataRef *ref = m_connection->subscribeDataRef(name, accuracy);
    connect(ref, &ClientDataRef::changed, this, &ExtPlaneClient::cdrChanged);
    connect(ref, &ClientDataRef::destroyed, this, &ExtPlaneClient::refDestroyed);
    ref->setClient(this);
    m_dataRefs.insert(ref);
    return ref;
}


void ExtPlaneClient::unsubscribeDataRefByName(QString name) {
    qDebug() << Q_FUNC_INFO << "Warning: this functions is deprecated and will be removed. Used with ref" << name;
    for(ClientDataRef *ref : m_dataRefs) {
        if(ref->name() == name) {
            m_dataRefs.erase(ref);
            m_connection->unsubscribeDataRef(ref);
            return;
        }
    }
}

void ExtPlaneClient::refDestroyed(QObject* refqo) {
    m_dataRefs.erase(static_cast<ClientDataRef*>(refqo));
}

void ExtPlaneClient::setConnectionMessage(QString msg) {
    m_connectionMessage = msg;
    emit connectionMessageChanged(m_connectionMessage);
}

void ExtPlaneClient::cdrChanged(ClientDataRef *ref) {
    if(ref->isArray()) {
        emit refChanged(ref->name(), ref->values());
    } else {
        // Try to convert to double and forward to corresponding slot per default
        // If that fails, we fallback to the string slot
        // TODO: Is this really a nice solution?
        bool ok;
        double value = ref->value().toDouble(&ok);
        if (ok) {
            emit refChanged(ref->name(), value);
        } else {
            emit refChanged(ref->name(), ref->value());
        }
    }
}

void ExtPlaneClient::unsubscribeDataRef(ClientDataRef *refToUnsubscribe) {
    for(ClientDataRef *ref : m_dataRefs) {
        if(ref->name() == refToUnsubscribe->name()) {
            m_dataRefs.erase(ref);
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
    if(m_heldButtons.find(id) == m_heldButtons.end()) return;
    m_heldButtons.erase(id);
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
    if(m_runningCommands.find(name) == m_runningCommands.end()) return;
    m_runningCommands.erase(name);
    m_connection->commandEnd(name);
}

ClientDataRefProvider *ExtPlaneClient::datarefProvider() const {
    return m_connection;
}

ExtPlaneConnection *ExtPlaneClient::extplaneConnection() {
    return (isSimulated() ? &m_simulatedExtplaneConnection : &m_extplaneConnection);
}

QString ExtPlaneClient::connectionMessage() {
    return m_connectionMessage;
}

bool ExtPlaneClient::isConnected() const
{
    return m_extplaneConnection.connected();
}

bool ExtPlaneClient::isSimulated() const {
    return m_simulated;
}

void ExtPlaneClient::setUpdateInterval(double newInterval) {
    Q_UNUSED(newInterval)
}

void ExtPlaneClient::setSimulated(bool simulated) {
    if (m_simulated == simulated)
        return;

    while(!m_dataRefs.empty())
        unsubscribeDataRef(*m_dataRefs.begin());

    qDebug() << Q_FUNC_INFO << simulated;

    m_simulated = simulated;
    m_connection = nullptr;
    emit datarefProviderChanged(m_connection);
    setConnectionMessage("");

    if(m_simulated) {
        m_extplaneConnection.stopConnection();
        m_connection = &m_simulatedExtplaneConnection;
        m_simulatedExtplaneConnection.startConnection();
    } else {
        m_simulatedExtplaneConnection.stopConnection();
        m_connection = &m_extplaneConnection;
        m_extplaneConnection.startConnection();
    }
    emit simulatedChanged(m_simulated);
    emit datarefProviderChanged(m_connection);
}

void ExtPlaneClient::valueSet(ClientDataRef *ref) {
    if(ref->modifiers().contains("string")) { // Quote strings
        m_connection->setValue(ref->name(), QString("\"%1\"").arg(ref->value()));
    }
    m_connection->setValue(ref->name(), ref->value());
}
