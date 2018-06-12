#!/bin/bash
#
# This script compiles ExtPlane plugin for windows 64bit from Linux host.
# Uses mxe cross-compile environment. Install mxe as debian packages first.
#
# Make sure XPlaneSDK is installed in directory next to ExtPlane.
#
# Example:
#
# src/ 
#    ExtPlane
#    XPlaneSDK
#
# Run this script from the ExtPlane dir:
#
# ./scripts/cross-compile-win64-from-lin.sh
#
#
# Good luck!
#

if [ ! -d ../ExtPlane ] ; then
   echo "Run this from ExtPlane subdirectory is. Read the comments in script."
   exit -1
fi

if [ ! -d /usr/lib/mxe ] ; then
   echo "mxe must be installed in /usr/lib/mxe!"
fi

PATH=/usr/lib/mxe/usr/bin/:$PATH

target=x86_64-w64-mingw32.static
mxedir=/usr/lib/mxe/

echo
echo Starting ExtPlane build
echo
$mxedir/usr/$target/qt5/bin/qmake -r
make

