#ifndef DEMOCLIENT_H
#define DEMOCLIENT_H

#include <QObject>
#include "extplaneclient.h"
#include "extplaneconnection.h"
#include "clientdataref.h"

class DemoClient : public QObject
{
    Q_OBJECT
public:
    explicit DemoClient(QObject *parent = nullptr);
    void run();
public slots:
    void connected();
    void refChanged(ClientDataRef *ref);
private:
    ExtPlaneClient m_client;
    ClientDataRef* m_secondsRef = nullptr;
    ClientDataRef* m_viewHeadingRef = nullptr;
    ClientDataRef* m_elevationRef = nullptr;
};

#endif // DEMOCLIENT_H
