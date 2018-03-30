#include "intarraydataref.h"
#include "../util/console.h"
#include <QStringList>

IntArrayDataRef::IntArrayDataRef(QObject *parent, QString name, void *ref) : DataRef(parent, name, ref) {
    _typeString = "ia";
    _type = extplaneRefTypeIntArray;
    _length = 0;
    _valueArray = nullptr;
}

IntArrayDataRef::~IntArrayDataRef() {
    if(_valueArray) delete [] _valueArray;
}

QVector<int> & IntArrayDataRef::value() {
    return _values;
}

void IntArrayDataRef::updateValue() {
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

QString IntArrayDataRef::valueString() {
    QString ret="[";

    for(int i=0;i<_length;i++){
        ret += QString::number(_values[i]) + (i < _length -1 ? QString(",") : "");
    }
    ret += "]";
    return ret;
}

void IntArrayDataRef::setValue(QString &newValue) {
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

    // Convert values to int and copy to _valueArray
    for(int i=0;i<numberOfValuesToWrite;i++) {
        bool ok = true;
        int value = values[i].toInt(&ok);
        if(!ok) {
            INFO << "Invalid value " << values.at(i) << "in array";
            return;
        }
        _valueArray[i]=value;
    }
    emit changed(this);
}

void IntArrayDataRef::setLength(int newLength)
{
    Q_ASSERT(newLength > 0);
    _values.fill(-9999, newLength); // Resize and initialize vector
    if(_valueArray) delete[] _valueArray;
    _valueArray = new int[newLength];
    _length = newLength;
}

int* IntArrayDataRef::valueArray()
{
    Q_ASSERT(_valueArray);
    return _valueArray;
}
