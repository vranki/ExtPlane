#include "extplaneclienttypes.h"
#include <QQmlEngine>
#include <extplaneclient.h>
#include <dataref.h>
#include <clientdatarefprovider.h>
#include <extplaneclient.h>
#include <dataref.h>
#include <extplaneconnection.h>
#include <QQmlContext>

void ExtPlaneClientTypes::registerQmlTypes()
{
    qmlRegisterInterface<ClientDataRefProvider>("ClientDataRefProvider");
    qmlRegisterType<ExtPlaneClient>("org.vranki.extplane", 1, 0, "ExtPlaneClient");
    qmlRegisterType<DataRef>("org.vranki.extplane", 1, 0, "DataRef");
    qmlRegisterType<ExtPlaneConnection>("org.vranki.extplane", 1, 0, "ExtPlaneConnection");
}

void ExtPlaneClientTypes::publishClient(QQmlContext *context, bool simulated)
{
    ExtPlaneClient *extplaneClient = new ExtPlaneClient(context, "extplaneClient", simulated);
    extplaneClient->createClient();
    context->setContextProperty("extplaneClient", extplaneClient);
}
