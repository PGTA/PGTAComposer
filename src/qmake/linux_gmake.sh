#!/bin/bash

runCommand()
{
    echo $1 && eval $1
    if [ "$?" != "0" ]; then
        echo "ERROR : $1" && exit 1
    fi
}

# capture current working directory
export QMAKEDIR=$(pwd)

# setup build directories
export BIN="$QMAKEDIR/../bin/"
export BUILD="$QMAKEDIR/gmake/"

# dependencies
export PGTA_PREMAKE="$QMAKEDIR/../external/PGTA/src/premake/"

# make gmake directory if it doesn't exist
test -d $BUILD || runCommand "mkdir -p $BUILD"

echo "--Building PGTA Composer Dependencies--"

pushd $PGTA_PREMAKE
    runCommand "./linux_gmake.sh"
popd

echo "--Creating Symbolic Links--"

pushd $build
    test -L lib_FlatBuffers.a || runCommand "ln -s $PGTA_PREMAKE/gmake/lib_FlatBuffers.a lib_FlatBuffers.a"
popd

echo "--Copying PGTA Binaries--"

# TODO : Copy library used to play/pause/stop PGTA track

echo "--Building PGTA Composer Application--"

runCommand 'qmake appsuite.pro'
runCommand 'make clean'
runCommand 'make'
