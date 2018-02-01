#!/bin/bash
#
# This script builds all versions of ExtPlane if
# possible. Currently builds lin & win versions.
#
# Creates a plugin zip package for release. 
#
# Run from ExtPlane directory.
#


# Build for linux first..
qmake -r
make clean distclean
qmake -r
make

# Build for windows..
./scripts/cross-compile-win64-from-lin.sh
pushd extplane-plugin
zip -r extplane.zip extplane
popd
pushd extplane-transformer
zip extplane-transformer.zip extplane-transformer_linux extplane-transformer.exe
popd

