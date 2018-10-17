#ifndef CLIENTDATAREF_H
#define CLIENTDATAREF_H

#include <QObject>
#include <QStringList>

class ExtPlaneClient;

/**
 * Represents a single dataref in client side. Can contain multiple
 * values, if ref is a array.
 *
 * You get a ClientDataRef by calling ExtPlaneConnection::subscribeDataRef().
 *
 * To unsubscribe, call ClientDataRef::unsubscribe().
 *
 * Never call delete or deleteLater() on this - memory is managed by ExtPlaneConnection.
 *
 * This is supposed to be used from C++. See DataRef class for QML friendly version.
 */
class ClientDataRef : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString value READ value WRITE setValue NOTIFY changed)
    Q_PROPERTY(QStringList values READ values WRITE setValues NOTIFY changed)
    Q_PROPERTY(double accuracy READ accuracy WRITE setAccuracy NOTIFY accuracyChanged)

public:
    explicit ClientDataRef(QObject *parent, QString newName, double accuracy=0);
    QString& name();
    QStringList& values(); // Returns all values
    double accuracy();
    QString value(); // Returns first value
    ExtPlaneClient* client() const;
    void updateValue(QString newValue); // Update single value (from simulator)
    void updateValue(QStringList &newValues); // Update full array (from simulator)
    int subscribers();
    void setSubscribers(int sub);
    bool isArray();
    void unsubscribe(); // Call to unsubscribe ref.

public slots:
    void setName(QString &name);
    void setAccuracy(double accuracy);
    void setValue(QString _newValue, int index=0); // Set value (from client)
    void setValues(QStringList values); // Set full array (from client)
    void setClient(ExtPlaneClient* client);

signals:
    void changed(ClientDataRef *ref); // Emitted when simulator updates value
    void valueSet(ClientDataRef *ref); // Emitted when client sets value
    void unsubscribed(ClientDataRef *ref);
    void nameChanged(QString name);
    void accuracyChanged(double accuracy);
    void clientChanged(ExtPlaneClient* client);

private:
    QString m_name;
    QStringList m_values; // Length 1 if not array
    double m_accuracy;
    int m_subscribers;
    ExtPlaneClient* m_client;
};

#endif // CLIENTDATAREF_H
