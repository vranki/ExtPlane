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
    size_t dataSize() override;
    void *rawData() override;
    void fromRawData(void *ptr) override;
private:
    int _value = 0;
};

#endif // INTDATAREF_H
