#!/bin/bash

qmake appsuite.pro

if [ "$?" != "0" ]; then
	echo "qmake ../appsuite.pro"
	exit 1
fi

make

if [ "$?" != "0" ]; then
	echo "make"
	exit 1
fi
