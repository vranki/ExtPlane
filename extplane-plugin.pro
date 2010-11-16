QT       += core network
QT       -= gui

TARGET = extplane-plugin
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = lib


SOURCES += main.cpp \
    xplaneplugin.cpp \
    dataref.cpp \
    tcpserver.cpp \
    tcpclient.cpp \
    datarefprovider.cpp \
    floatdataref.cpp \
    intdataref.cpp \
    doubledataref.cpp

INCLUDEPATH += $$(HOME)/SDK/CHeaders/XPLM

QMAKE_CXXFLAGS_DEBUG  += -D LIN=1 -D XPLM200=1  -fPIC -shared -rdynamic -nodefaultlibs
QMAKE_CXXFLAGS_RELEASE  += -D LIN=1 -D XPLM200=1  -fPIC -shared -rdynamic -nodefaultlibs
QMAKE_LFLAGS += -shared

HEADERS += \
    xplaneplugin.h \
    dataref.h \
    tcpserver.h \
    tcpclient.h \
    datarefprovider.h \
    floatdataref.h \
    intdataref.h \
    doubledataref.h

OTHER_FILES += README
