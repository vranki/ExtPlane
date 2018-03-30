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
    _valueArray = nullptr;
}

FloatArrayDataRef::~FloatArrayDataRef() {
    if(_valueArray) delete [] _valueArray;
}

QVector<float> & FloatArrayDataRef::value() {
    return _values;
}

// Copies values from valuearray to value list and emits changed as needed
void FloatArrayDataRef::updateValue() {
    Q_ASSERT(_length > 0);
    bool valuesChanged = false;
    for(int i=0;i<_length;i++){
        if(_values.at(i) != _valueArray[i]) {
            _values.replace(i, _valueArray[i]);
            valuesChanged = true;
        }
    }
    if (valuesChanged) {
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
        INFO << "Invalid array value" << newValue;
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
            INFO << "Invalid value " << values.at(i) << "in array";
            return;
        }
        _valueArray[i]=value;
    }
    updateValue();
}

void FloatArrayDataRef::setLength(int newLength)
{
    Q_ASSERT(newLength > 0);
    _values.fill(-9999, newLength); // Resize and initialize vector
    if(_valueArray) delete[] _valueArray;
    _valueArray = new float[newLength];
    _length = newLength;
}

float *FloatArrayDataRef::valueArray()
{
    Q_ASSERT(_valueArray);
    return _valueArray;
}
