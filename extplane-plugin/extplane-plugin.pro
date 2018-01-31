include(../common.pri)

message("Building X-Plane plugin with SDK in $$XPLANE_SDK_PATH")

#
# ExtPlane plugin build configuration
#

# X-Plane SDK version. The example shows ALL can be 1 at the same time.
# Defaults to SDK 3.0.0 (X-Plane 11)
QMAKE_CXXFLAGS += -DXPLM300=1
QMAKE_CXXFLAGS += -DXPLM210=1
QMAKE_CXXFLAGS += -DXPLM200=1

INCLUDEPATH += $$XPLANE_SDK_PATH/CHeaders/XPLM
INCLUDEPATH += ../extplane-server
DEPENDPATH += . ../extplane-server
INCLUDEPATH += $$PWD/../util/
LIBS += -L../extplane-server -lextplane-server

# You should not need to touch anything below this for normal build

# Detailed X-Plane plugin build instructions here:
# http://www.xsquawkbox.net/xpsdk/mediawiki/BuildInstall

QT       += core network
QT       -= gui

CONFIG   += console warn_on shared c++11
CONFIG   -= app_bundle
CONFIG   -= debug_and_release

TEMPLATE = lib

TARGET = extplane-plugin

QMAKE_LFLAGS += -shared -fPIC
#  -static-libgcc  <- fails on mac

unix:!macx {
    message("Linux Platform")
    DEFINES += APL=0 IBM=0 LIN=1
    QMAKE_CXXFLAGS += -fPIC
    XPLDIR = extplane/64
    XPLFILE = lin.xpl
}

macx {
     message("Mac Platform")
     DEFINES += APL=1 IBM=0 LIN=0
     QMAKE_LFLAGS += -dynamiclib
     # -flat_namespace -undefined warning <- not needed or recommended anymore.

     # Build for multiple architectures.
     # The following line is only needed to build universal on PPC architectures.
     # QMAKE_MAC_SDK=/Devloper/SDKs/MacOSX10.4u.sdk
     # This line defines for wich architectures we build.
     CONFIG += x86 ppc
     QMAKE_LFLAGS += -F$$XPLANE_SDK_PATH/Libraries/Mac
     QMAKE_CXXFLAGS += -fPIC
     LIBS += -framework XPLM
     XPLDIR = extplane
     XPLFILE = mac.xpl
}

win32 {
    DEFINES += APL=0 IBM=1 LIN=0
    DEFINES += NOMINMAX #Qt5 bug
    LIBS += -L$$XPLANE_SDK_PATH/Libraries/Win
# We should test for target arch, not host arch, but this doesn't work. Fix.
#    !contains(QMAKE_TARGET.arch, x86_64) {
    XPLFILE = win.xpl
    !contains(QMAKE_HOST.arch, x86_64) {
        message("Windows 32 bit Platform")
        LIBS += -lXPLM -lXPWidgets
        XPLDIR = extplane/32
    } else {
        message("Windows 64 bit Platform")
        LIBS += -lXPLM_64 -lXPWidgets_64
        XPLDIR = extplane/64
    }
}

message("Plugin file will be copied to $$XPLDIR/$$XPLFILE")

CONFIG(debug, debug|release) {
    # Debug
    message("ExtPlane Debug Build")
    debug.DESTDIR = $$DESTDIR
} else {
    # Release
    message("ExtPlane Release Build")
    DEFINES += QT_NO_DEBUG
    DEFINES += QT_NO_DEBUG_OUTPUT
    release.DESTDIR = $$DESTDIR
}

# Copy the built library to the correct x-plane plugin directory
QMAKE_POST_LINK += $(MKDIR) $$XPLDIR ; $(COPY_FILE) $(TARGET) $$XPLDIR/$$XPLFILE

SOURCES += main.cpp \
    xplaneplugin.cpp \
    customdata/navcustomdata.cpp \
    customdata/atccustomdata.cpp
HEADERS += \
    xplaneplugin.h \
    customdata/navcustomdata.h \
    customdata/atccustomdata.h \

