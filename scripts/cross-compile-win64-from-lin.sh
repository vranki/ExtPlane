#!/bin/bash

# NOTE: THIS IS WIP, doesn't work yet. Feel free to fix.

# This script compiles ExtPlane plugin for windows from Linux host.
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
# good luck!

# This fails if it's already installed and that's ok:
git clone https://github.com/mxe/mxe.git
pushd mxe
# Make sure we have up to date mxe installation..
git pull
# This takes long time, if first installation:
make qt5
popd
PATH=${PWD}/mxe/usr/bin:$PATH
pushd ExtPlane
make clean distclean
../mxe/usr/i686-w64-mingw32.static/qt5/bin/qmake
make
popd

