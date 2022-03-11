#include "intarraydataref.h"
#include "../../util/console.h"
#include <QStringList>
#include <algorithm>

IntArrayDataRef::IntArrayDataRef(QObject *parent, QString name, void *ref) : DataRef(parent, name, ref) {
    _typeString = "ia";
    _type = extplaneRefTypeIntArray;
    _length = 0;
    _valueArray = nullptr;
    changedIndices = new std::list<indexPair>;
}

IntArrayDataRef::~IntArrayDataRef() {
    if(_valueArray) delete [] _valueArray;
    if(changedIndices) delete changedIndices;
}

std::vector<int> & IntArrayDataRef::value() {
    return _values;
}

void IntArrayDataRef::updateValue() {
    bool valuesChanged = false;
    for(int i=0;i<_length;i++){
        if(_values.at(i) != _valueArray[i]) {
            _values[i] = _valueArray[i];
            valuesChanged = true;
        }
    }
    if (valuesChanged || !isValid()) {
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
    indexPair indPair;

    indPair.lower = 0;
    indPair.upper = 0;

    // Check that value starts with [ and ends with ]
    if(!newValue.startsWith('[') || !newValue.endsWith(']')) {
        INFO << "Invalid array value" << newValue;
        return;
    }

    changedIndices->clear();

    // Remove [] and split values
    QString arrayString = newValue.mid(1, newValue.length() - 2);
    QStringList values = arrayString.split(',');

    // Limit number of values to write to ref length or number of given values
    int numberOfValuesToWrite = qMin(_length, values.size());

    // Convert values to int and copy to _valueArray
    for(int i=0;i<numberOfValuesToWrite;i++) {
        bool ok = true;
        int value = values[i].toInt(&ok);
        if(ok) {
            _valueArray[i] = value;
            if(changedIndices->empty() || changedIndices->back().upper != (i-1))
            {
              indPair.lower = i;
              indPair.upper = i;
              changedIndices->push_back(indPair);
            } else {    
              changedIndices->back().upper = i;
            }
        } else if(!values.at(i).isEmpty()) {
            INFO << "Invalid value " << values.at(i) << "in array";
        }
    }
    updateValue();
}

void IntArrayDataRef::setLength(int newLength)
{
    Q_ASSERT(newLength > 0);
    std::fill(_values.begin(), _values.end(), -9999);
    if(_valueArray) delete[] _valueArray;
    _valueArray = new int[newLength];
    _length = newLength;
}

int* IntArrayDataRef::valueArray()
{
    Q_ASSERT(_valueArray);
    return _valueArray;
}
