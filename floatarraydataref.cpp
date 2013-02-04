/*
 *  floatarraydataref.cpp
 *  extplane-plugin
 *
 *  Created by bobgates on 2011/07/23.
 *  Copyright 2011 DaffeySoft. All rights reserved.
 *
 */

#include "floatarraydataref.h"

FloatArrayDataRef::FloatArrayDataRef(QObject *parent, QString name, XPLMDataRef ref) : DataRef(parent, name, ref), _value(NULL)
{
    _typeString = "fa";
    _type = xplmType_FloatArray;
    
    _length = (float) XPLMGetDatavf(_ref, NULL, 0, 0);
    qDebug() << Q_FUNC_INFO << "Inited dataref with a length of =" << _length;
    
    _value = (float *)calloc(_length+1, sizeof(float));
    _value[0] = (float) _length;
}

float * FloatArrayDataRef::value() {
    return _value;
}

void FloatArrayDataRef::updateValue() {
    float newValue[_length];
    
    long retval = XPLMGetDatavf(_ref, newValue, 0, _length);
    //    qDebug() << Q_FUNC_INFO << "XPLMGetDatavf returns" << retval;
    
    bool notequal = false;
    for(long i=0;i<_length;i++){
        
        if(_value[i+1] != newValue[i]) {
            _value[i+1] = newValue[i];
            notequal = true;
            // break;
        }
    }
    if (notequal) emit changed(this);
}


void FloatArrayDataRef::setValue(float newValue) {
    qDebug() << Q_FUNC_INFO << "Interface currenly doesn't allow setting Datavf type" << name();
    return;
/*
    if(!isWritable()) {
        qDebug() << Q_FUNC_INFO << "Tried to write read-only dataref" << name();
        return;
    }
    qDebug() << Q_FUNC_INFO << name() << newValue;
    _value = newValue;
    XPLMSetDataf(ref(), _value);
*/
}

QString FloatArrayDataRef::valueString() {
    QString ret = QString::number(_length)+QString(": ");
    
    for(int i=0;i<_length;i++){
        ret += QString::number(_value[i+1])+QString(", ");
    }
    
    return ret;
}

void FloatArrayDataRef::setValue(QString &newValue) {
    qDebug() << Q_FUNC_INFO << "Interface currenly doesn't allow setting Datavf type" << name();
    return;
/*    
    bool ok = false;
    float value = newValue.toFloat(&ok);
    if(ok) {
        setValue(value);
    } else {
        qDebug() << Q_FUNC_INFO << "Cannot set value " << newValue;
    }
*/
}
