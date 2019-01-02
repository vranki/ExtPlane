TEMPLATE = lib
CONFIG += staticlib c++11

include(../common.pri)

# Needed for windows mxe build (hope this doesn't break anything..)
DESTDIR = $$PWD

QT       += core network
QT       -= gui

INCLUDEPATH += $$PWD/../util/

SOURCES += tcpserver.cpp \
    tcpclient.cpp

HEADERS += \
    tcpserver.h \
    tcpclient.h \
    ../util/console.h
