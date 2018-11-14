QT       += network
QT       -= gui

TEMPLATE = lib
CONFIG += staticlib c++11
INCLUDEPATH += $$PWD/../../util/

HEADERS += \
    dataref.h \
    simulatedextplaneconnection.h \
    extplaneconnection.h \
    extplaneclient.h \
    clientdatarefprovider.h \
    clientdataref.h \
    simulateddatarefs/alternatingsimulateddataref.h \
    simulateddatarefs/fixedsimulateddataref.h \
    simulateddatarefs/simulateddataref.h \
    ../../util/basictcpclient.h \

SOURCES += \
    dataref.cpp \
    simulatedextplaneconnection.cpp \
    extplaneconnection.cpp \
    extplaneclient.cpp \
    clientdatarefprovider.cpp \
    clientdataref.cpp \
    simulateddatarefs/alternatingsimulateddataref.cpp \
    simulateddatarefs/fixedsimulateddataref.cpp \
    simulateddatarefs/simulateddataref.cpp \
    ../../util/basictcpclient.cpp
