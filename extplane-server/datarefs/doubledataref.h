#ifndef DOUBLEDATAREF_H
#define DOUBLEDATAREF_H

#include "dataref.h"

class DoubleDataRef : public DataRef {
    Q_OBJECT

public:
    DoubleDataRef(QObject *parent, QString name, void* ref);
    double value();
    virtual void updateValue(double newValue);
    virtual QString valueString();
    virtual void setValue(QString &newValue);
private:
    double _value;
};

#endif // DOUBLEDATAREF_H
