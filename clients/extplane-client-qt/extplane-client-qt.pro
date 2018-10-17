QT       += network
QT       -= gui

TEMPLATE = lib
CONFIG += c++11

INCLUDEPATH += $$PWD/../../util/

HEADERS += \
    simulatedextplaneconnection.h \
    extplaneconnection.h \
    extplaneclient.h \
    clientdatarefprovider.h \
    clientdataref.h \
    simulateddatarefs/alternatingsimulateddataref.h \
    simulateddatarefs/fixedsimulateddataref.h \
    simulateddatarefs/simulateddataref.h \
    ../../util/basictcpclient.h \
    dataref.h

SOURCES += \
    simulatedextplaneconnection.cpp \
    extplaneconnection.cpp \
    extplaneclient.cpp \
    clientdatarefprovider.cpp \
    clientdataref.cpp \
    simulateddatarefs/alternatingsimulateddataref.cpp \
    simulateddatarefs/fixedsimulateddataref.cpp \
    simulateddatarefs/simulateddataref.cpp \
    ../../util/basictcpclient.cpp \
    dataref.cpp
