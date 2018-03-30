#ifndef DATADATAREF_H
#define DATADATAREF_H

#include "dataref.h"

#include <QByteArray>
#include <QTime>

/**
  * Represents a Binary DataRef (type b).
  * The accuracy for binary datarefs represents how often ExtPlane
  * should check for updates in milliseconds. When the accuracy
  * is set to 0, ExtPlane will check for updates as soon as possible.
  *
  * Defaults to base64 encoding.
  * Use modifier "string" to return text as string.
  */
class DataDataRef : public DataRef {
    Q_OBJECT

public:
    DataDataRef(QObject *parent, QString &name, void* ref);
    QByteArray &value();
    QByteArray &newValue(); // Write to this and call updatevalue to change value
    void setValue(QByteArray &newValue); // @todo implement
    virtual void updateValue();
    virtual QString valueString();
    virtual void setValue(QString &newValue);
    void setLength(int newLength);
private:
    QByteArray _value; // Value of dataref is stored here once retrieved from XPLM
    QByteArray _newValue; // Temp variable used while checking changes. Here to avoid resizing.
    QTime _lastUpdate; // Timer used for tracking the last update time
};

#endif // DATADATAREF_H
