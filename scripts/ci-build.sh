#!/bin/bash
#
# This script builds all versions of ExtPlane if
# possible. Currently builds lin & win versions.
#
# Creates a plugin zip package for release. 
#
# Run from ExtPlane directory.
#

# Get X-Plane SDK if needed..
if [ ! -d ../XPlaneSDK/CHeaders ] ; then
  rm -rf ../XPlaneSDK
  wget http://developer.x-plane.com/wp-content/plugins/code-sample-generation/sample_templates/XPSDK300.zip
  unzip *.zip
  rm XPSDK300.zip
  mv SDK ../XPlaneSDK
else
  echo "X-Plane SDK already exists"
fi

# Build for linux first..
qmake -r
make
zip -r extplane-plugin.zip extplane-plugin/extplane/64/*.xpl
zip extplane-transformer.zip extplane-transformer/extplane-transformer-linux extplane-transformer.exe
make clean distclean

# Build for windows..
./scripts/cross-compile-win64-from-lin.sh
zip -r extplane-plugin.zip extplane-plugin/extplane/64/*.xpl
zip extplane-transformer.zip extplane-transformer/extplane-transformer.exe

