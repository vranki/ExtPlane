#ifndef DATADATAREF_H
#define DATADATAREF_H

#include "dataref.h"

#include <QByteArray>

class DataDataRef : public DataRef {
    Q_OBJECT

public:
    DataDataRef(QObject *parent, QString name, XPLMDataRef ref);
    QByteArray &value();
    void setValue(QByteArray &newValue);
    virtual void updateValue();
    virtual QString valueString();
    virtual void setValue(QString &newValue);
private:
    int _length; // Length of dataref, as given by X-Plane
    char *_byteArray;
    QByteArray _value;
};

#endif // DATADATAREF_H
