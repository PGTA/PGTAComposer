#!/bin/bash

BUILD_DIRECTORY=./gmake/

if [ ! -d "$BUILD_DIRECTORY" ]; then
  mkdir $BUILD_DIRECTORY
fi

pushd gmake/

qmake ../pgta_composer.pro

if [ "$?" != "0" ]; then
	echo "qmake ../pgta_composer.pro"
	exit 1
fi

make

if [ "$?" != "0" ]; then
	echo "make"
	exit 1
fi
popd
