#!/bin/sh

runCommand()
{
    echo $1 && $1
    if [ "$?" != "0" ]; then
        echo "ERROR : $1" && exit 1
    fi
}

bin="./../bin/"
gmake="./gmake/"
pgta_premake="./../external/PGTA/src/premake/"

echo "--Building PGTA Composer Dependencies--"

pushd $pgta_premake

runCommand "./osx_gmake.sh"

popd

echo "--Copying PGTA Binaries--"

runCommand "mkdir -p $gmake"
runCommand "cp -r $pgta_premake/gmake/* $gmake"
runCommand "mkdir -p $bin"
runCommand "cp -r $pgta_premake/../../bin/* $bin"

echo "--Building PGTA Composer Application--"

runCommand 'qmake appsuite.pro'
runCommand 'make clean'
runCommand 'make'
