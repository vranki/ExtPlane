#include "datadataref.h"

DataDataRef::DataDataRef(QObject *parent, QString name, XPLMDataRef ref) : DataRef(parent, name, ref)
{
    _typeString = "b";
    _type = xplmType_Data;
    _length = XPLMGetDatab(_ref, NULL, 0, 0);
    qDebug() << Q_FUNC_INFO << "Inited dataref with a length of =" << _length;
    //_values.fill(-9999, _length); // Resize and initialize vector
    _value = QByteArray(_length,0);
    //_valueArray = new float[_length];
}

QByteArray &DataDataRef::value() {
    return _value;
}

void DataDataRef::updateValue() {
    QByteArray newValue(_length,0);
    int valuesCopied = XPLMGetDatab(_ref, newValue.data(), 0, _length);
    Q_ASSERT(valuesCopied == _length);

    bool notequal = false;
    for(int i=0;i<_length;i++){
        if(_value[i] != newValue[i]) {
            _value[i] = newValue[i];
            notequal = true;
        }
    }
    if (notequal) emit changed(this);
}

void DataDataRef::setValue(QByteArray &newValue) {
    /*
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

    // Convert values to float and copy to _valueArray
    for(int i=0;i<numberOfValuesToWrite;i++) {
        bool ok = true;
        float value = values[i].toFloat(&ok);
        if(!ok) {
            qDebug() << Q_FUNC_INFO << "Invalid value " << values[i] << "in array";
            return;
        }
        _valueArray[i]=value;
    }
    XPLMSetDatavf(_ref, _valueArray, 0, numberOfValuesToWrite);
    */
}

QString DataDataRef::valueString() {
    return QString(_value).toUtf8().toBase64();
}

void DataDataRef::setValue(QString &newValue) {
    /*
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

    // Convert values to float and copy to _valueArray
    for(int i=0;i<numberOfValuesToWrite;i++) {
        bool ok = true;
        float value = values[i].toFloat(&ok);
        if(!ok) {
            qDebug() << Q_FUNC_INFO << "Invalid value " << values[i] << "in array";
            return;
        }
        _valueArray[i]=value;
    }
    XPLMSetDatavf(_ref, _valueArray, 0, numberOfValuesToWrite);*/
}
