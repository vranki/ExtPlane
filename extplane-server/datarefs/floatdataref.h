#ifndef FLOATDATAREF_H
#define FLOATDATAREF_H

#include "dataref.h"

class FloatDataRef : public DataRef {
    Q_OBJECT
public:
    FloatDataRef(QObject *parent, QString &name, void* ref);
    float value();
    void updateValue(float newValue);
    virtual QString valueString();
    void setValue(QString & newValue);
private:
    float _value = -999999.0f;
};

#endif // FLOATDATAREF_H
