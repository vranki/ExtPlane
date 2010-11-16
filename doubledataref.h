#ifndef DOUBLEDATAREF_H
#define DOUBLEDATAREF_H

#include <dataref.h>

class DoubleDataRef : public DataRef {
    Q_OBJECT

public:
    DoubleDataRef(QObject *parent, QString name, XPLMDataRef ref);
    double value();
    void setValue(double newValue);
    virtual void updateValue();
    virtual QString valueString();
    virtual void setValue(QString &newValue);
private:
    double _value;
};

#endif // DOUBLEDATAREF_H
