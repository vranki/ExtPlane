#ifndef FLOATDATAREF_H
#define FLOATDATAREF_H

#include "dataref.h"

class FloatDataRef : public DataRef {
    Q_OBJECT

public:
    FloatDataRef(QObject *parent, QString name, XPLMDataRef ref);
    float value();
    void setValue(float newValue);
    virtual void updateValue();
    virtual QString valueString();
    virtual void setValue(QString &newValue);
private:
    float _value;
};

#endif // FLOATDATAREF_H
