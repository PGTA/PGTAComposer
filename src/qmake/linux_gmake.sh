#!/bin/bash

runCommand()
{
    echo $1 && $1
    if [ "$?" != "0" ]; then
        echo "ERROR : $1" && exit 1
    fi
}

echo "--Building PGTA Composer Dependencies--"

runCommand './../external/build-tools/bin/premake/premake5_linux clean --file=externals.lua'
runCommand './../external/build-tools/bin/premake/premake5_linux gmake --file=externals.lua'

pushd gmake/

runCommand 'make clean'
runCommand 'make'

popd

echo "--Building PGTA Composer Applicatoin--"

runCommand 'qmake appsuite.pro'
runCommand 'make clean'
runCommand 'make'
