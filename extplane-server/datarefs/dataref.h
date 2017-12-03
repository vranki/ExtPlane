#ifndef DATAREF_H
#define DATAREF_H

#include <QString>
#include <QObject>
#include <QDebug>
//#include "XPLMDataAccess.h"

//typedef void* XPLMDataRef;
typedef int extplaneRefID;

// Ref types. These match X-Plane's ref type values.
enum {
    extplaneRefTypeUnknown = 0,
    extplaneRefTypeInt = 1,
    extplaneRefTypeFloat = 2,
    extplaneRefTypeDouble = 4,
    extplaneRefTypeFloatArray = 8,
    extplaneRefTypeIntArray = 16,
    extplaneRefTypeData = 32
};


/**
  * Base class for DataRefs.
  */
class DataRef : public QObject {
    Q_OBJECT
public:
    DataRef(QObject *parent, QString name, void* ref);
    QString &name();
    void* ref();
    int subscribers();
    void setSubscribers(int subs);
    void setWritable(bool cw);
    bool isWritable();
    virtual QString valueString() = 0;
    virtual void setValue(QString &newValue) = 0;
    extplaneRefID type(); // NOTE: always only one type, although XPLMDataTypeID can have many.
    virtual void setType(extplaneRefID newType); // Only set after constructor
    QString typeString();
    virtual void setAccuracy(double val);
    virtual void updateAccuracy(double val);
    double accuracy() { return _accuracy; }
signals:
    void changed(DataRef *ref);
protected:
    QString _typeString;
    extplaneRefID _type;
    void* _ref;
    double _accuracy;
private:
    QString _name;
    int _subscribers;
    bool _writable;
};

#endif // DATAREF_H
