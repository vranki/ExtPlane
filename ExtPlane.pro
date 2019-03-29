TEMPLATE = subdirs
SUBDIRS = extplane-server \
    clients/extplane-client-qt
CONFIG += ordered

include(common.pri)

defined(XPLANE_SDK_PATH, var) {
    message("Building X-Plane plugin with SDK in $$XPLANE_SDK_PATH")
    SUBDIRS += extplane-plugin
} else {
    warning("No X-Plane SDK found in ../XPlaneSDK or ~/SDK - not building X-Plane plugin")
}

versionAtLeast(QT_VERSION, 5.8.0) {
    SUBDIRS += extplane-transformer
} else {
    warning(Qt 5.8.0 needed to build extplane-transformer - skipping $$QT_VERSION)
}

OTHER_FILES += README.md clients/extplane-client-qt/README
