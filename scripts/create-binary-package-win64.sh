
# This script creates a package for Windows Platform with all dependencies.
# It is based on http://qt-project.org/doc/qt-5.0/qtdoc/deployment-windows.html
# and http://www.xsquawkbox.net/xpsdk/mediawiki/64_Bit
# This is what it does:
#    * creates a package folder for the version and platform specified below
#    * copies ExtPlane resources
#    * copies needed Qt resources (dlls)
#    * copies needed Qt plugins
#    * copies needed MSVC plugins
#    * creates a ZIP file of the whole thing and cleans up
# The results are a package that contains everything needed to run ExtPlane on a fresh
# install of Windows.

# Package config
VERSION="0.1"
PLATFORM="win"
ARC="64"

# Tools config
MSVC_PATH="resources"
QT_BIN_PATH="C:/Qt/Qt5.0.2/5.0.2/msvc2012_64/bin"
QT_PLUGIN_PATH="C:/Qt/Qt5.0.2/5.0.2/msvc2012_64/plugins"
EXTPLANE_PROJECT_PATH="C:/Users/admin/ExtPlane"
EXTPLANE_BIN_PATH="C:/Users/admin/build-extplane-plugin-Qt5_64bit-Release/release"
EXTPLANE_PACKAGE_PATH="C:/Users/admin/ExtPlane-Packages/ExtPlane-Plugin-v$VERSION/ExtPlane/$ARC"

# Show config
echo " "
echo "VERSION = $VERSION";
echo "PLATFORM = $PLATFORM";
echo "MSVC_PATH = $MSVC_PATH";
echo "QT_BIN_PATH = $QT_BIN_PATH";
echo "QT_PLUGIN_PATH = $QT_PLUGIN_PATH";
echo "EXTPLANE_BIN_PATH = $EXTPLANE_BIN_PATH";
echo "EXTPLANE_PACKAGE_PATH = $EXTPLANE_PACKAGE_PATH";

# Verify
echo " "
echo "Press any key to continue"
echo " "
read

echo "Creating package folder..."
#rm -Rf $EXTPLANE_PACKAGE_PATH
mkdir -p $EXTPLANE_PACKAGE_PATH

echo "Copying ExtPlane resources..."
cp -f $EXTPLANE_BIN_PATH/extplane-plugin.dll $EXTPLANE_PACKAGE_PATH/win.xpl
cp -f $EXTPLANE_BIN_PATH/extplane-plugin.dll.embed.manifest $EXTPLANE_PACKAGE_PATH/win.xpl.embed.manifest

echo "Copying Qt resources..."
cp -f $QT_BIN_PATH/Qt5Core.dll $EXTPLANE_PACKAGE_PATH
cp -f $QT_BIN_PATH/Qt5Network.dll $EXTPLANE_PACKAGE_PATH
cp -f $QT_BIN_PATH/icuin49.dll $EXTPLANE_PACKAGE_PATH
cp -f $QT_BIN_PATH/icuuc49.dll $EXTPLANE_PACKAGE_PATH
cp -f $QT_BIN_PATH/icudt49.dll $EXTPLANE_PACKAGE_PATH

echo "Copying Qt plugins..."
mkdir -p $EXTPLANE_PACKAGE_PATH/platforms
cp -f $QT_PLUGIN_PATH/platforms/qminimal.dll $EXTPLANE_PACKAGE_PATH/platforms

echo "Copying MSVC resources..."
cp -f $MSVC_PATH/msvcp110.dll.x86_64 $EXTPLANE_PACKAGE_PATH/msvcp110.dll
cp -f $MSVC_PATH/msvcr110.dll.x86_64 $EXTPLANE_PACKAGE_PATH/msvcr110.dll

