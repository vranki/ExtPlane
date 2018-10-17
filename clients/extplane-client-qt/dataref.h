#ifndef DATAREF_H
#define DATAREF_H

#include <QObject>

class ClientDataRef;
class ExtPlaneClient;
class DataRefProvicer;

/**
 * Use this class to create datarefs from QML code and wherever you
 * want. About the same API as with ClientDataRef.
 *
 * Will automatically connect when name is set.
 */
class DataRef : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString value READ value WRITE setValue NOTIFY changed)
    Q_PROPERTY(QStringList values READ values WRITE setValues NOTIFY changed)
    Q_PROPERTY(double accuracy READ accuracy WRITE setAccuracy NOTIFY accuracyChanged)
    Q_PROPERTY(ExtPlaneClient* client READ client WRITE setClient NOTIFY clientChanged)

public:
    explicit DataRef(QObject *parent = nullptr);
    QString& name();
    QStringList& values(); // Returns all values
    double accuracy();
    QString value(); // Returns first value
    ExtPlaneClient* client() const;

signals:
    void changed(ClientDataRef *ref); // Emitted when simulator updates value
    void valueSet(ClientDataRef *ref); // Emitted when client sets value
    void unsubscribed(ClientDataRef *ref);
    void nameChanged(QString name);
    void accuracyChanged(double accuracy);
    void clientChanged(ExtPlaneClient* client);

public slots:
    void setName(QString &name);
    void setAccuracy(double accuracy);
    void setValue(QString _newValue, int index=0); // Set value (from client)
    void setValues(QStringList values); // Set full array (from client)
    void setClient(ExtPlaneClient* client);
    void setDataRefProvider();

private:
    void subscribeIfPossible();
    ClientDataRef *m_clientDataRef;
    ExtPlaneClient *m_client;
    QString m_name;
    QStringList m_emptyStringList; // Return if no client dataref available yet
    double m_accuracy;
};

#endif // DATAREF_H
