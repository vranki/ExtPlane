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


/**
  * Base class for DataRefs.
  */
class DataRef : public QObject {
    Q_OBJECT
public:
    DataRef(QObject *parent, QString name, void* ref);
    const QString &name() const;
    void* ref();
    int subscriberCount() const;
    void setSubscriberCount(const int subs);
    void setWritable(const bool cw);
    bool isWritable();
    virtual QString valueString() = 0;
    virtual void setValue(QString &newValue) = 0;
    extplaneRefID type() const; // NOTE: always only one type, although XPLMDataTypeID can have many.
    virtual void setType(extplaneRefID newType); // Only set after constructor
    QString typeString() const;
    virtual void setAccuracy(double val);
    virtual void updateAccuracy(double val);
    double accuracy() { return _accuracy; }
    void setUnsubscribeAfterChange();
    bool shouldUnsubscribeAfterChange() const;
    bool isValid() const; // True if the value has been set initially. False if not.

signals:
    void changed(DataRef *ref); // Should not be emitted if value is not valid.

protected:
    void setValueValid(); // Call this to mark the value valid.

    QString _typeString;
    extplaneRefID _type;
    void* _ref;
    double _accuracy;
    bool _valueValid;

private:
    QString _name;
    int _subscriberCount;
    bool _writable;
    bool _unsubscribeAfterChange;
};

#endif // DATAREF_H
