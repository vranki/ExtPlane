#include "intarraydataref.h"
#include <QStringList>

IntArrayDataRef::IntArrayDataRef(QObject *parent, QString name, XPLMDataRef ref) : DataRef(parent, name, ref) {
    _typeString = "ia";
    _type = xplmType_IntArray;
    _length = XPLMGetDatavi(_ref, NULL, 0, 0);
    qDebug() << Q_FUNC_INFO << "Inited dataref with a length of =" << _length;
    _values.fill(-9999, _length); // Resize and initialize vector
    _valueArray = new int[_length];
}

IntArrayDataRef::~IntArrayDataRef() {
    delete [] _valueArray;
}

QVector<int> & IntArrayDataRef::value() {
    return _values;
}

void IntArrayDataRef::updateValue() {
    int valuesCopied = XPLMGetDatavi(_ref, _valueArray, 0, _length);
    Q_ASSERT(valuesCopied == _length);

    bool notequal = false;
    for(int i=0;i<_length;i++){
        if(_values[i] != _valueArray[i]) {
            _values[i] = _valueArray[i];
            notequal = true;
        }
    }
    if (notequal) emit changed(this);
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
        qDebug() << Q_FUNC_INFO << "Invalid array value";
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
            qDebug() << Q_FUNC_INFO << "Invalid value " << values[i] << "in array";
            return;
        }
        _valueArray[i]=value;
    }
    XPLMSetDatavi(_ref, _valueArray, 0, numberOfValuesToWrite);
}
