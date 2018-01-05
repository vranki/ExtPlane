#!/bin/bash
#
# This script compiles ExtPlane plugin for windows 64bit from Linux host.
# Uses mxe cross-compile environment.
#
# Make sure XPlaneSDK is installed in directory next to ExtPlane.
# mxe will be downloaded if it doesn't exist.
#
# Example:
#
# src/ 
#    ExtPlane
#    mxe
#    XPlaneSDK
#
# Run this script from the src dir:
#
# ./ExtPlane/scripts/cross-compile-win64-from-lin.sh
#
# If mxe complains about some dependencies missing, please
# install them.
#
# Good luck!
#

if [ ! -d ExtPlane ] ; then
   echo "Run this from directory where ExtPlane subdirectory is. Read the comments in script."
   exit -1
fi

# Get mxe if needed..
if [ ! -d mxe ] ; then
  git clone https://github.com/mxe/mxe.git
  pushd mxe
  # This takes long time, if first installation:
  make MXE_TARGETS=x86_64-w64-mingw32.static -j`nproc` qt5
  popd
fi

PATH=${PWD}/mxe/usr/bin:$PATH
pushd ExtPlane
echo
echo Cleaning up ExtPlane directory..
echo
make clean distclean
echo
echo Starting ExtPlane build
echo
../mxe/usr/x86_64-w64-mingw32.static/qt5/bin/qmake -r
make
popd

