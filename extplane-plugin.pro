QT       += core network
QT       -= gui

CONFIG   += console warn_on release shared
CONFIG   -= app_bundle

TEMPLATE = lib

TARGET = extplane-plugin

QMAKE_CXXFLAGS_DEBUG += -DXPLM200=1 -fPIC
QMAKE_CXXFLAGS_RELEASE += -DXPLM200=1 -fPIC
QMAKE_LFLAGS += -shared -static-libgcc -fPIC

unix:!macx {
     DEFINES += APL=0 IBM=0 LIN=1
     QMAKE_CFLAGS += -fstack-protector
     QMAKE_CXXFLAGS += -fstack-protector #-rdynamic -nodefaultlibs
     QMAKE_LFLAGS += -shared
}

macx {
     DEFINES += APL=1 IBM=0 LIN=0
     QMAKE_LFLAGS += -flat_namespace -undefined warning -dynamiclib
     # Build for multiple architectures.
     # The following line is only needed to build universal on PPC architectures.
     # QMAKE_MAC_SDK=/Devloper/SDKs/MacOSX10.4u.sdk
     # This line defines for wich architectures we build.
     CONFIG += x86 ppc
}

QMAKE_POST_LINK += $(COPY_FILE) $(TARGET) extplane.xpl
QMAKE_CLEAN += extplane.xpl

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

