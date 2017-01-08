QT       += network
QT       -= gui

TEMPLATE = lib

HEADERS += \
    simulatedextplaneconnection.h \
    extplaneconnection.h \
    extplaneclient.h \
    clientdatarefprovider.h \
    clientdataref.h \
    simulateddatarefs/alternatingsimulateddataref.h \
    simulateddatarefs/fixedsimulateddataref.h \
    simulateddatarefs/simulateddataref.h

SOURCES += \
    simulatedextplaneconnection.cpp \
    extplaneconnection.cpp \
    extplaneclient.cpp \
    clientdatarefprovider.cpp \
    clientdataref.cpp \
    simulateddatarefs/alternatingsimulateddataref.cpp \
    simulateddatarefs/fixedsimulateddataref.cpp \
    simulateddatarefs/simulateddataref.cpp

