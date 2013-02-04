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

class FloatArrayDataRef : public DataRef {
    Q_OBJECT
    
public:
    FloatArrayDataRef(QObject *parent, QString name, XPLMDataRef ref);
    float * value();
    void setValue(float newValue);
    virtual void updateValue();
    virtual QString valueString();
    virtual void setValue(QString &newValue);
private:
    float * _value;
    long _length;
};

#endif // FloatArrayDataRef_H
