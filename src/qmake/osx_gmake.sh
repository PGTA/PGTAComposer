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
    test -L libPGTATestCommon_x64.a || runCommand "ln -s $pgta_premake/gmake/libPGTATestCommon_x64.a libPGTATestCommon_x64.a"
popd

echo "--Copying PGTA Binaries--"

runCommand "cp -rf $pgta_premake/gmake/libPGTALib.dylib $build/"
runCommand "cp -rf $pgta_premake/gmake/libSDL2.dylib $build/"
runCommand "cp -rf $pgta_premake/gmake/libSDL2-2.0.0.dylib $build/"

echo "--Building PGTA Composer Application--"
runCommand 'qmake appsuite.pro'
runCommand 'make clean'
runCommand 'make'
