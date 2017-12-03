#ifndef INTDATAREF_H
#define INTDATAREF_H

#include "dataref.h"

class IntDataRef : public DataRef {
    Q_OBJECT

public:
    IntDataRef(QObject *parent, QString name, void* ref);
    int value();
    void setValue(int newValue);
    virtual void updateValue(int newValue);
    virtual QString valueString();
    virtual void setValue(QString &newValue);
private:
    int _value;
};

#endif // INTDATAREF_H
