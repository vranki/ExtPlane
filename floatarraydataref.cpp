/*
 *  floatarraydataref.cpp
 *  extplane-plugin
 *
 *  Created by bobgates on 2011/07/23.
 *  Copyright 2011 DaffeySoft. All rights reserved.
 *
 */

#include "floatarraydataref.h"

FloatArrayDataRef::FloatArrayDataRef(QObject *parent, QString name, XPLMDataRef ref) : DataRef(parent, name, ref) {
    _typeString = "fa";
    _type = xplmType_FloatArray;
    _length = (float) XPLMGetDatavf(_ref, NULL, 0, 0);
    qDebug() << Q_FUNC_INFO << "Inited dataref with a length of =" << _length;
    _values.fill(0, _length); // Resize vector
    _valueArrayForReading = new float[_length];
}

FloatArrayDataRef::~FloatArrayDataRef() {
    delete [] _valueArrayForReading;
}

QVector<float> & FloatArrayDataRef::value() {
    return _values;
}

void FloatArrayDataRef::updateValue() {
    long valuesCopied = XPLMGetDatavf(_ref, _valueArrayForReading, 0, _length);
    Q_ASSERT(valuesCopied == _length);

    bool notequal = false;
    for(long i=0;i<_length;i++){
        if(_values[i] != _valueArrayForReading[i]) {
            _values[i] = _valueArrayForReading[i];
            notequal = true;
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
        ret += QString::number(_values[i])+QString(", ");
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
