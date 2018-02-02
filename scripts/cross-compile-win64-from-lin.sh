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
# Run this script from the ExtPlane dir:
#
# ./scripts/cross-compile-win64-from-lin.sh
#
# If mxe complains about some dependencies missing, please
# install them.
#
# Good luck!
#

if [ ! -d ../ExtPlane ] ; then
   echo "Run this from ExtPlane subdirectory is. Read the comments in script."
   exit -1
fi

if [ ! -d ../mxe ] ; then
   echo "mxe must be installed in ../mxe!"
fi

PATH=${PWD}/../mxe/usr/bin:$PATH
echo
echo Cleaning up ExtPlane directory..
echo
make clean distclean
echo
echo Starting ExtPlane build
echo
../mxe/usr/x86_64-w64-mingw32.static/qt5/bin/qmake -r
make

