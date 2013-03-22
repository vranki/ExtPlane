#ifndef CLIENTDATAREFPROVIDER_H
#define CLIENTDATAREFPROVIDER_H

#include "clientdataref.h"

/**
 * Interface common for a dataref provider classes
 *
 * @see ExtPlaneConnection
 * @see SimulatedExtPlaneConnection
 */
class ClientDataRefProvider
{
public:
    ClientDataRefProvider();
    virtual ClientDataRef *subscribeDataRef(QString name, double accuracy=0)=0;
    virtual void unsubscribeDataRef(ClientDataRef *ref)=0;
    virtual void keyPress(int id)=0;
    virtual void buttonPress(int id)=0;
    virtual void buttonRelease(int id)=0;
    virtual void setValue(QString name, QString value)=0; // For single value
    virtual void setValues(QString name, QStringList values)=0; // For arrays
};

#endif // CLIENTDATAREFPROVIDER_H
