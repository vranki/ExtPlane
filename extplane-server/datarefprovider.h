#ifndef DATAREFPROVIDER_H
#define DATAREFPROVIDER_H
#include <QString>

class DataRef;

enum extplaneCommandType {
    extplaneCommandTypeInvalid = 0,
    extplaneCommandTypeOnce,
    extplaneCommandTypeBegin,
    extplaneCommandTypeEnd,
};

/**
  * Interface class for subscribing and unsubscribing datarefs
  */
class DataRefProvider {
public:
    DataRefProvider();
    virtual ~DataRefProvider();
    // Subscribe to a named dataref and return pointer to a DataRef instance
    virtual DataRef *subscribeRef(QString name)=0;
    // Unsubscribe the given dataref
    virtual void unsubscribeRef(DataRef *ref)=0;

    // Update ref value from simulator
    virtual void updateDataRef(DataRef *ref)=0;
    // Change ref value to simulator
    virtual void changeDataRef(DataRef *ref)=0;

    virtual void keyStroke(int keyid)=0;
    virtual void buttonPress(int buttonid)=0;
    virtual void buttonRelease(int buttonid)=0;

    virtual void command(QString &name, extplaneCommandType type)=0;
};

#endif // DATAREFPROVIDER_H
