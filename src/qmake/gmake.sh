#!/bin/bash

BUILD_DIRECTORY=./gmake/

if [ ! -d "$BUILD_DIRECTORY" ]; then
  mkdir $BUILD_DIRECTORY
fi

pushd gmake/
qmake ../pgta_composer.pro
make clean
make
popd
