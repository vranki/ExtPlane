/*
 *  floatarraydataref.cpp
 *  extplane-plugin
 *
 *  Created by bobgates on 2011/07/23.
 *  Modified by Ville Ranki <ville.ranki@iki.fi>
 */

#include "floatarraydataref.h"
#include "../util/console.h"
#include <QStringList>

FloatArrayDataRef::FloatArrayDataRef(QObject *parent, QString name, void *ref) : DataRef(parent, name, ref) {
    _typeString = "fa";
    _type = extplaneRefTypeFloatArray;
    _length = 0;
    _valueArray = 0;
}

FloatArrayDataRef::~FloatArrayDataRef() {
    if(_valueArray) delete [] _valueArray;
}

QVector<float> & FloatArrayDataRef::value() {
    return _values;
}

// Copies values from valuearray to value list and emits changed as needed
void FloatArrayDataRef::updateValue() {
    bool notequal = false;
    for(int i=0;i<_length;i++){
        if(_values[i] != _valueArray[i]) {
            _values[i] = _valueArray[i];
            notequal = true;
        }
    }
    if (notequal) {
        if(!_valueValid) setValueValid();
        emit changed(this);
    }
}

QString FloatArrayDataRef::valueString() {
    QString ret="[";

    for(int i=0;i<_length;i++){
        ret += QString::number(_values[i]) + (i < _length -1 ? QString(",") : "");
    }
    ret += "]";
    return ret;
}

void FloatArrayDataRef::setValue(QString &newValue) {
    // Check that value starts with [ and ends with ]
    if(!newValue.startsWith('[') || !newValue.endsWith(']')) {
        INFO << "Invalid array value";
        return;
    }

    // Remove [] and split values
    QString arrayString = newValue.mid(1, newValue.length() - 2);
    QStringList values = arrayString.split(',');

    // Limit number of values to write to ref length or number of given values
    int numberOfValuesToWrite = qMin(_length, values.size());

    // Convert values to float and copy to _valueArray
    for(int i=0;i<numberOfValuesToWrite;i++) {
        bool ok = true;
        float value = values[i].toFloat(&ok);
        if(!ok) {
            INFO << "Invalid value " << values[i] << "in array";
            return;
        }
        _valueArray[i]=value;
    }
    updateValue();
}

void FloatArrayDataRef::setLength(int newLength)
{
    _values.fill(-9999, _length); // Resize and initialize vector
    if(_valueArray) delete[] _valueArray;
    _valueArray = new float[_length];
}

float *FloatArrayDataRef::valueArray()
{
    return _valueArray;
}
