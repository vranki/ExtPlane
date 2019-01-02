exists($$PWD/../XPlaneSDK) {
  XPLANE_SDK_PATH=$$PWD/../XPlaneSDK
}
exists($$(HOME)/SDK/) {
  XPLANE_SDK_PATH=$$(HOME)/SDK/
}

# Build all the dataref classes as they are needed
# everywhere.

SOURCES += \
    $$PWD/extplane-server/datarefprovider.cpp \
    $$PWD/extplane-server/datarefs/dataref.cpp \
    $$PWD/extplane-server/datarefs/floatdataref.cpp \
    $$PWD/extplane-server/datarefs/floatarraydataref.cpp \
    $$PWD/extplane-server/datarefs/intdataref.cpp \
    $$PWD/extplane-server/datarefs/doubledataref.cpp \
    $$PWD/extplane-server/datarefs/intarraydataref.cpp \
    $$PWD/extplane-server/datarefs/datadataref.cpp

HEADERS += \
    $$PWD/extplane-server/datarefprovider.h \
    $$PWD/extplane-server/datarefs/dataref.h \
    $$PWD/extplane-server/datarefs/floatdataref.h \
    $$PWD/extplane-server/datarefs/floatarraydataref.h \
    $$PWD/extplane-server/datarefs/intdataref.h \
    $$PWD/extplane-server/datarefs/doubledataref.h \
    $$PWD/extplane-server/datarefs/intarraydataref.h \
    $$PWD/extplane-server/datarefs/datadataref.h
