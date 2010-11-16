#ifndef DATAREF_H
#define DATAREF_H

#include <QString>
#include <QObject>
#include <QDebug>
#include "XPLMDataAccess.h"

/**
  * Base class for DataRefs.
  */
class DataRef : public QObject {
    Q_OBJECT
public:
    DataRef(QObject *parent, QString name, XPLMDataRef ref);
    QString &name();
    XPLMDataRef ref();
    int subscribers();
    void setSubscribers(int subs);
    virtual void updateValue() = 0;
    void setCanWrite(bool cw);
    bool isWritable();
    virtual QString valueString() = 0;
    virtual void setValue(QString &newValue) = 0;
    XPLMDataTypeID type(); // NOTE: always only one type, although XPLMDataTypeID can have many.
    QString typeString();
signals:
    void changed(DataRef *ref);
protected:
    QString _typeString;
    XPLMDataTypeID _type;
private:
    QString _name;
    XPLMDataRef _ref;
    int _subscribers;
    bool _writable;
};

#endif // DATAREF_H
