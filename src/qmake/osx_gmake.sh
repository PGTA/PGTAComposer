#!/bin/sh

runCommand()
{
    echo $1 && eval $1
    if [ "$?" != "0" ]; then
        echo "ERROR : $1" && exit 1
    fi
}

# capture current working directory
qmakedir=$(pwd)

# setup build directories
bin="$qmakedir/../bin/"
build="$qmakedir/gmake/"

# dependencies
pgta_premake="$qmakedir/../external/PGTA/src/premake/"

# make gmake directory if it doesn't exist
test -d $build || runCommand "mkdir -p $build"

echo "--Building PGTA Composer Dependencies--"

pushd $pgta_premake
    runCommand "./osx_gmake.sh"
popd

echo "--Creating Symbolic Links--"

pushd $build
    test -L lib_FlatBuffers.a || runCommand "ln -s $pgta_premake/gmake/lib_FlatBuffers.a lib_FlatBuffers.a"
popd

echo "--Copying PGTA Binaries--"

# TODO : Copy library used to play/pause/stop PGTA track

echo "--Building PGTA Composer Application--"

runCommand 'qmake appsuite.pro'
runCommand 'make clean'
runCommand 'make'
