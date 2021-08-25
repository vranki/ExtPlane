TEMPLATE = lib
CONFIG += staticlib c++11

include(../common.pri)

# Needed for windows mxe build (hope this doesn't break anything..)
DESTDIR = $$PWD
# Yes - it breaks shadow build. Fix the mxe build.

QT       += core network
QT       -= gui

INCLUDEPATH += $$PWD/../util/

SOURCES += tcpserver.cpp tcpclient.cpp udpsender.cpp

HEADERS += \
    tcpserver.h \
    tcpclient.h \
    udpsender.h \
    ../util/console.h

