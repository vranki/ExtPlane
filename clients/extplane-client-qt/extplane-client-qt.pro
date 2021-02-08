QT       -= gui

TEMPLATE = lib
CONFIG += staticlib c++11
INCLUDEPATH += $$PWD/../../util/

include(extplane-client-qt.pri)

HEADERS += \
    democlient.h

SOURCES += \
    democlient.cpp

OTHER_FILES += README.md

SUBDIRS += \
    democlient.pro
