#ifndef DATAREFPROVIDER_H
#define DATAREFPROVIDER_H

#include "dataref.h"
/**
  * Interface class for subscribing and unsubscribing datarefs
  */
class DataRefProvider {
public:
    DataRefProvider();
    virtual DataRef *subscribeRef(QString name)=0;
    virtual void unsubscribeRef(DataRef *ref)=0;
    virtual void keyStroke(int keyid)=0;
    virtual void buttonPress(int buttonid)=0;
    virtual void buttonRelease(int buttonid)=0;
};

#endif // DATAREFPROVIDER_H
