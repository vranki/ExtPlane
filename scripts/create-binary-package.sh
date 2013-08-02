
# This script creates a package for all platforms by copying the already-built plugin results along
# with ExtPlane resources (such as the readme).


# Package config
VERSION="0.1"

# Tools config
EXTPLANE_PROJECT_PATH="C:/Users/admin/ExtPlane"
EXTPLANE_PACKAGE_PATH="C:/Users/admin/ExtPlane-Packages/ExtPlane-Plugin-v$VERSION"

# Show config
echo " "
echo "VERSION = $VERSION";
echo "EXTPLANE_PACKAGE_PATH = $EXTPLANE_PACKAGE_PATH";

# Verify
echo " "
echo "Press any key to continue"
echo " "
read

echo "Copying ExtPlane resources..."
cp -f $EXTPLANE_PROJECT_PATH/README $EXTPLANE_PACKAGE_PATH

echo "Creating archive..."
rm -f $EXTPLANE_PACKAGE_PATH.zip
tools/7za.exe a $EXTPLANE_PACKAGE_PATH.zip $EXTPLANE_PACKAGE_PATH