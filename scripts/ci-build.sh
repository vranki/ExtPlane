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
make clean distclean

# Build for windows..
./scripts/cross-compile-win64-from-lin.sh

# Zip the results for release
pushd extplane-plugin
zip -r extplane.zip extplane
popd
pushd extplane-transformer
zip extplane-transformer.zip extplane-transformer-linux extplane-transformer.exe
popd

