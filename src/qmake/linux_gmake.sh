#!/bin/bash

runCommand()
{
    echo $1 && $1
    if [ "$?" != "0" ]; then
        echo "ERROR : $1" && exit 1
    fi
}

export BIN="./../bin/"
export GMAKE="./gmake/"
export PGTA_PREMAKE="./../external/PGTA/src/premake/"

echo "--Building PGTA Composer Dependencies--"

pushd $PGTA_PREMAKE

runCommand "./linux_gmake.sh"

popd

echo "--Copying PGTA Binaries--"

runCommand "mkdir -p $GMAKE"
runCommand "cp -r $PGTA_PREMAKE/gmake/* $GMAKE"
runCommand "mkdir -p $BIN"
runCommand "cp -r $PGTA_PREMAKE/../../bin/* $BIN"

echo "--Building PGTA Composer Application--"

runCommand 'qmake appsuite.pro'
runCommand 'make clean'
runCommand 'make'
