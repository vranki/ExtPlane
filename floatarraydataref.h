/*
 *  floatarraydataref.h
 *  extplane-plugin
 *
 *  Created by bobgates on 2011/07/23.
 *  Copyright 2011 DaffeySoft. All rights reserved.
 *
 */

#ifndef FloatArrayDataRef_H
#define FloatArrayDataRef_H

#include "dataref.h"
#include <QVector>

class FloatArrayDataRef : public DataRef {
    Q_OBJECT
    
public:
    FloatArrayDataRef(QObject *parent, QString name, XPLMDataRef ref);
    ~FloatArrayDataRef();
    QVector<float> & value();
    void setValue(float newValue);
    virtual void updateValue();
    virtual QString valueString();
    virtual void setValue(QString &newValue);
private:
    QVector<float> _values; // Actual values in a vector
    long _length; // Length of dataref, as given by X-Plane
    float * _valueArrayForReading; // Array for reading values. Allocated dynamically.
};

#endif // FloatArrayDataRef_H
