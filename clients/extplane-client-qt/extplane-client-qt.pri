QT += quick network

INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/dataref.h \
    $$PWD/extplaneclienttypes.h \
    $$PWD/extplaneudpclient.h \
    $$PWD/simulatedextplaneconnection.h \
    $$PWD/extplaneconnection.h \
    $$PWD/extplaneclient.h \
    $$PWD/clientdatarefprovider.h \
    $$PWD/clientdataref.h \
    $$PWD/simulateddatarefs/alternatingsimulateddataref.h \
    $$PWD/simulateddatarefs/fixedsimulateddataref.h \
    $$PWD/simulateddatarefs/simulateddataref.h \
    $$PWD/../../util/basictcpclient.h \

SOURCES += \
    $$PWD/dataref.cpp \
    $$PWD/extplaneclienttypes.cpp \
    $$PWD/extplaneudpclient.cpp \
    $$PWD/simulatedextplaneconnection.cpp \
    $$PWD/extplaneconnection.cpp \
    $$PWD/extplaneclient.cpp \
    $$PWD/clientdatarefprovider.cpp \
    $$PWD/clientdataref.cpp \
    $$PWD/simulateddatarefs/alternatingsimulateddataref.cpp \
    $$PWD/simulateddatarefs/fixedsimulateddataref.cpp \
    $$PWD/simulateddatarefs/simulateddataref.cpp \
    $$PWD/../../util/basictcpclient.cpp
