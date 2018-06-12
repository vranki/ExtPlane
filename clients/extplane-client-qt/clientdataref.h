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
 */
class ClientDataRef : public QObject {
    Q_OBJECT
    Q_PROPERTY(ExtPlaneClient* client READ client WRITE setClient NOTIFY clientChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString value READ value WRITE setValue NOTIFY changed)
    Q_PROPERTY(QStringList values READ values WRITE setValues NOTIFY changed)
    Q_PROPERTY(double accuracy READ accuracy WRITE setAccuracy NOTIFY accuracyChanged)

public:
    explicit ClientDataRef();
    explicit ClientDataRef(QObject *parent, QString newName, double accuracy=0);
    QString& name();
    QStringList& values(); // Returns all values
    double accuracy();
    void updateValue(QString newValue); // Update single value (from simulator)
    void updateValue(QStringList &newValues); // Update full array (from simulator)
    int subscribers();
    void setSubscribers(int sub);
    bool isArray();
    void unsubscribe(); // Call to unsubscribe ref.
    QString value(); // Returns first value
    ExtPlaneClient* client() const;

public slots:
    void setName(QString &name);
    void setAccuracy(double accuracy);
    void setValue(QString _newValue, int index=0);
    void setValues(QStringList values);
    void setClient(ExtPlaneClient* client);

signals:
    void changed(ClientDataRef *ref);
    void valueSet(ClientDataRef *ref);
    void unsubscribed(ClientDataRef *ref);
    void nameChanged(QString name);
    void accuracyChanged(double accuracy);
    void clientChanged(ExtPlaneClient* client);

private:
    void subscribeIfPossible();

    QString m_name;
    QStringList m_values;
    double m_accuracy;
    int m_subscribers;
    ExtPlaneClient* m_client;
};

#endif // CLIENTDATAREF_H
