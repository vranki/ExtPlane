/*
 *  floatarraydataref.h
 *  extplane-plugin
 *
 *  Created by bobgates on 2011/07/23.
 *  Modified by Ville Ranki <ville.ranki@iki.fi>
 */

#ifndef FloatArrayDataRef_H
#define FloatArrayDataRef_H

#include "dataref.h"
#include <vector>
#include <list>

class FloatArrayDataRef : public DataRef {
    Q_OBJECT
    
public:
    FloatArrayDataRef(QObject *parent, QString name, void* ref);
    ~FloatArrayDataRef();
    std::vector<float> &value();
    std::list<std::pair<int, int>> changedIndices;
    virtual void updateValue();
    virtual QString valueString();
    virtual void setValue(QString &newValue);
    void setLength(int newLength);
    float *valueArray();
private:
    std::vector<float> _values; // Actual values in a vector
    int _length; // Length of dataref, as given by X-Plane. 0 if not set.
    float *_valueArray; // Array for reading & writing values. Allocated dynamically.
};

#endif // FloatArrayDataRef_H
