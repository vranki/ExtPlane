#ifndef IntArrayDataRef_H
#define IntArrayDataRef_H

#include "dataref.h"
#include <QObject>
#include <vector>

class IntArrayDataRef : public DataRef {
    Q_OBJECT

public:
    IntArrayDataRef(QObject *parent, QString name, void* ref);
    ~IntArrayDataRef();
    std::vector<int> &value();
    std::list<indexPair> changedIndices;
    virtual void updateValue();
    virtual QString valueString();
    virtual void setValue(QString &newValue);
    void setLength(int newLength);
    int *valueArray();
private:
    std::vector<int> _values; // Actual values in a vector
    int _length; // Length of dataref, as given by X-Plane
    int *_valueArray; // Array for reading & writing values. Allocated dynamically.
};

#endif // IntArrayDataRef_H
