
#NOTE: This is not yet working...

echo "ExtPlane Fat Plugin Assembler"

# Config
VERSION="0.1"
PLATFORM="osx"
ARC="32"

QT_LIB_PATH="/Users/dankrusi/Qt5.0.1/5.0.1/clang_64/lib"
EXTPLANE_PROJECT_PATH="/Users/dankrusi/Code/ExtPlane"
EXTPLANE_BIN_PATH="/Users/dankrusi/Code/extplane-plugin-build-Desktop_Qt_5_0_1_clang_32bit-Default_build_Release"
PLUGIN_NAME="ExtPlane"
PACKAGE_PATH="/Users/dankrusi/Code/ExtPlane-Packages/$PLUGIN_NAME-Plugin-v$VERSION/$PLUGIN_NAME"
XPLANE_PLUGIN_PATH="/Users/dankrusi/Desktop/XPlane10/Resources/plugins"

# Show config
echo " "
echo "VERSION = $VERSION";
echo "PLATFORM = $PLATFORM";
echo "ARC = $ARC";
echo "QT_LIB_PATH = $QT_LIB_PATH";
echo "EXTPLANE_PROJECT_PATH = $EXTPLANE_PROJECT_PATH";
echo "PACKAGE_PATH = $PACKAGE_PATH";

# Verify
echo " "
echo "Press any key to continue"
echo " "
read

echo "Copying ExtPlane resources..."
mkdir -p $PACKAGE_PATH/$ARC
cp $EXTPLANE_BIN_PATH/extplane.xpl $PACKAGE_PATH/$ARC/mac.xpl
cp $EXTPLANE_PROJECT_PATH/README $PACKAGE_PATH

echo "Copying Qt Frameworks..."
cp -R $QT_LIB_PATH/QtCore.framework $PACKAGE_PATH/$ARC/
cp -R $QT_LIB_PATH/QtNetwork.framework $PACKAGE_PATH/$ARC/

echo "Change framework ids..."
LINK_PATH="@executable_path/../../../Resources/plugins/$PLUGIN_NAME/$ARC"
install_name_tool -id $LINK_PATH/QtCore.framework/Versions/5/QtCore $PACKAGE_PATH/$ARC/QtCore.framework/Versions/5/QtCore
install_name_tool -id $LINK_PATH/QtNetwork.framework/Versions/5/QtNetwork $PACKAGE_PATH/$ARC/QtNetwork.framework/Versions/5/QtNetwork

# Change framework paths in QtNetwork
echo "Changing framework paths in QtNetwork.framework..."
install_name_tool -change $QT_LIB_PATH/QtCore.framework/Versions/5/QtCore $LINK_PATH/QtCore.framework/Versions/5/QtCore $PACKAGE_PATH/$ARC/QtNetwork.framework/Versions/5/QtNetwork

echo "Changing framework paths in xpl..."
install_name_tool -change $QT_LIB_PATH/QtCore.framework/Versions/5/QtCore $LINK_PATH/QtCore.framework/Versions/5/QtCore $PACKAGE_PATH/$ARC/mac.xpl
install_name_tool -change $QT_LIB_PATH/QtNetwork.framework/Versions/5/QtNetwork $LINK_PATH/QtNetwork.framework/Versions/5/QtNetwork $PACKAGE_PATH/$ARC/mac.xpl
#install_name_tool -change @executable_path/../../../Resources/plugins/XPLM.framework/XPLM @executable_path/../../XPLM.framework/XPLM $PACKAGE_PATH/$ARC/mac.xpl

#TODO: strip debug symbols from frameworks

# Show bindings...
#otool -L $PACKAGE_PATH/$ARC/mac.xpl

