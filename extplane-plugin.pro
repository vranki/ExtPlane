#
# ExtPlane plugin build configuration
#
# Change to -DXPLM200=1 for SDK 2.0.0 (X-Plane 9)
QMAKE_CXXFLAGS += -DXPLM210=1

# Set this to path where the X-Plane SDK is
INCLUDEPATH += $$(HOME)/SDK/CHeaders/XPLM

# You should not need to touch anything below this for normal build

# Detailed X-Plane plugin build instructions here:
# http://www.xsquawkbox.net/xpsdk/mediawiki/BuildInstall

QT       += core network
QT       -= gui

CONFIG   += console warn_on release shared
CONFIG   -= app_bundle

TEMPLATE = lib

TARGET = extplane-plugin

QMAKE_CXXFLAGS += -fPIC
QMAKE_LFLAGS += -shared -static-libgcc -fPIC

unix:!macx {
     DEFINES += APL=0 IBM=0 LIN=1
     QMAKE_CFLAGS += -fstack-protector
     QMAKE_CXXFLAGS += -fstack-protector
     QMAKE_LFLAGS += -shared -rdynamic -nodefaultlibs -undefined_warning
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
    tcpserver.cpp \
    tcpclient.cpp \
    datarefprovider.cpp \
    datarefs/dataref.cpp \
    datarefs/floatdataref.cpp \
    datarefs/floatarraydataref.cpp \
    datarefs/intdataref.cpp \
    datarefs/doubledataref.cpp \
    datarefs/intarraydataref.cpp

HEADERS += \
    xplaneplugin.h \
    tcpserver.h \
    tcpclient.h \
    datarefprovider.h \
    datarefs/dataref.h \
    datarefs/floatdataref.h \
    datarefs/floatarraydataref.h \
    datarefs/intdataref.h \
    datarefs/doubledataref.h \
    datarefs/intarraydataref.h

OTHER_FILES += README client/extplane-client-qt/README
