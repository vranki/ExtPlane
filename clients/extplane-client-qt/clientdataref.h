#ifndef CLIENTDATAREF_H
#define CLIENTDATAREF_H

#include <QObject>
#include <QStringList>
#include <cmath>

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
 *
 * dataFormat variable contains hints how to interpret data in ambigous situations.
 *
 * Current values:
 * (empty) - default behavior
 * (binary) - Returns binary data dataref in an array of bytes, that you get with values()
 *
 */
class ClientDataRef : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString value READ value WRITE setValue NOTIFY changed)
    Q_PROPERTY(QStringList values READ values WRITE setValues NOTIFY changed)
    Q_PROPERTY(double accuracy READ accuracy WRITE setAccuracy NOTIFY accuracyChanged)
    Q_PROPERTY(QString dataFormat READ dataFormat WRITE setDataFormat NOTIFY dataFormatChanged)

public:
    explicit ClientDataRef(QObject *parent, QString newName, double accuracy=0);
    virtual ~ClientDataRef();
    QString& name();
    QStringList& values(); // Returns all values
    double accuracy();
    QString value(); // Returns first value
    int valueInt();
    float valueFloat();
    double valueDouble();
    ExtPlaneClient* client() const;
    void updateValue(QString newValue); // Update single value (from simulator)
    void updateValue(QStringList &newValues); // Update full array (from simulator)
    void updateValue(int newValue);
    void updateValue(float newValue);
    void updateValue(double newValue);
    int subscribers();
    void setSubscribers(int sub);
    bool isArray();
    void unsubscribe(); // Call to unsubscribe ref. Subscriber count will be reduced by one.
    const QString &dataFormat() const;
    const QStringList &modifiers() const;
    void setUdpId(quint16 id);
    quint16 udpId();

public slots:
    void setName(QString &name);
    void setAccuracy(double accuracy);
    void setValue(QString _newValue, int index=0); // Set value (from client)
    void setValues(QStringList values); // Set full array (from client)
    void setClient(ExtPlaneClient* client);
    void setDataFormat(QString dataFormat);

signals:
    void changed(ClientDataRef *ref); // Emitted when simulator updates value
    void valueSet(ClientDataRef *ref); // Emitted when client sets value
    void unsubscribed(ClientDataRef *ref); // Emitted when one subscriber unsubscribes (not necessarily all)
    void nameChanged(QString name);
    void accuracyChanged(double accuracy);
    void clientChanged(ExtPlaneClient* client);
    void dataFormatChanged(QString dataFormat);

private slots:
    void clientDestroyed();

private:
    QString valueString(); // Generate string from raw values, if possible
    void invalidateValue();
    QString m_name;
    QStringList m_values; // Length 1 if not array
    int m_valueInt = 0;
    bool m_valueIntValid = false;
    float m_valueFloat = std::nanf("");
    double m_valueDouble = std::nan("");
    double m_accuracy;
    int m_subscribers = 0;
    ExtPlaneClient* m_client = nullptr;
    QString m_dataFormat;
    QStringList m_modifiers;
    quint16 m_udpId = 0;
};

#endif // CLIENTDATAREF_H
