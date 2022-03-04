#ifndef DATAREF_H
#define DATAREF_H

#include <QString>
#include <QObject>
#include <QDebug>

typedef int extplaneRefID;

// Ref types. These match X-Plane's ref type values.
enum {
    extplaneRefTypeUnknown = 0,
    extplaneRefTypeInt = 1,
    extplaneRefTypeFloat = 2,
    extplaneRefTypeDouble = 4,
    extplaneRefTypeFloatArray = 8,
    extplaneRefTypeIntArray = 16,
    extplaneRefTypeData = 32
};

typedef struct {
    int lower;
    int upper;
} indexPair;

/**
  * Base class for DataRefs.
  */
class DataRef : public QObject {
    Q_OBJECT
public:
    DataRef(QObject *parent, const QString &name, void* ref);
    const QString &name() const;
    const QStringList &modifiers() const;
    void* ref() const;
    int subscriberCount() const;
    void setSubscriberCount(const int subs);
    void setWritable(const bool cw);
    bool isWritable() const;
    virtual QString valueString() = 0;
    virtual void setValue(QString &newValue) = 0;
    extplaneRefID type() const; // NOTE: always only one type, although XPLMDataTypeID can have many.
    virtual void setType(extplaneRefID newType); // Only set after constructor
    const QString &typeString() const;
    virtual void setAccuracy(double val);
    virtual void updateAccuracy(double val);
    double accuracy() const { return _accuracy; }
    void setUnsubscribeAfterChange(); // Call to mark this ref to be unsubscribed after one change
    bool shouldUnsubscribeAfterChange() const;
    bool isValid() const; // True if the value has been set initially. False if not.
    void setUdpId(quint16 id);
    quint16 udpId();
    // Raw data processing - Use only for int, float and double refs for now.
    virtual size_t dataSize(); // Number of bytes the value requires - eg float and int = 4, double 8, others return 0.
    virtual void* rawData(); // Raw pointer to ref data. Null if not supported.
    virtual void fromRawData(void *ptr); // Set ref value from raw pointer. Reads dataSize() bytes.

signals:
    void changed(DataRef *ref); // Should not be emitted if value is not valid.

protected:
    void setValueValid(); // Call this to mark the value valid.

    QString _typeString = "?";
    extplaneRefID _type = extplaneRefTypeUnknown;
    void* _ref = nullptr;
    double _accuracy = 0;
    bool _valueValid = false;

private:
    QString _name;
    QStringList _modifiers;
    int _subscriberCount = 0;
    bool _writable = false;
    bool _unsubscribeAfterChange = false;
    quint16 _udpId = 0; // UDP id, global for all clients
};

#endif // DATAREF_H
