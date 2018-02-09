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
  mv SDK ../XPlaneSDK
else
  echo "X-Plane SDK already exists and contains:"
  ls -lh ../XPlaneSDK
fi

# For some reason mxe dir stays after cache clean, make sure it's deleted or valid
if [ ! -f ../mxe/.git ] ; then
  rm -rf ../mxe
fi

# Get mxe if needed..
# (broken atm, need to recompile mxe in 16.04 container..)
#if [ ! -d ../mxe ] ; then
#  pushd ..
#  wget http://www.modeemi.fi/~cosmo/mxe.tar.gz
#  tar xvfz mxe.tar.gz
#  git clone https://github.com/mxe/mxe.git
#  popd
#fi

# Build mxe if needed
#pushd ../mxe
#  echo mxe dir:
#  ls
#  git pull
#  make MXE_TARGETS=x86_64-w64-mingw32.static -j`nproc` qt5
#popd

# Build for linux first..
qmake -r
make

# Build for windows..
#./scripts/cross-compile-win64-from-lin.sh

# Zip the results for release
pushd extplane-plugin
zip -r extplane.zip extplane
popd
pushd extplane-transformer
zip extplane-transformer.zip extplane-transformer-linux 
#extplane-transformer.exe
popd

