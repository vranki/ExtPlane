# Qt ExtPlane client library

## Setting up project

Set up qmake variable EXTPLANE to point to extplane sources. Then in in your .pro file: 

```
include($${EXTPLANE}/clients/extplane-client-qt/extplane-client-qt.pri)
```

## Usage in QML

In main.cpp that initializes the QML types:

```cpp
#include <extplaneclienttypes.h>
```

And in the main function, when ready to register QML types call:

```cpp
ExtPlaneClientTypes::registerQmlTypes();
ExtPlaneClientTypes::publishClient(engine.rootContext());
```

Engine is a QQmlApplicationEngine instance you are using.

In QML code import the components and set the host name like this:


```qml
import org.vranki.extplane 1.0

...

    Component.onCompleted: extplaneClient.extplaneConnection.hostName = "127.0.0.1"

    Connections {
        target: extplaneClient
        onConnectionMessageChanged: console.log("ExtPlane client status:", connectionMessage)
    }
```

The client will try to connect to the plugin server.

