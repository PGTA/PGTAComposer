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
export BIN="$QMAKEDIR/../../bin/"
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

pushd $BUILD
    test -L lib_FlatBuffers.a || runCommand "ln -s $PGTA_PREMAKE/gmake/lib_FlatBuffers.a lib_FlatBuffers.a"
    test -L libPGTATestCommon_x64.a || runCommand "ln -s $PGTA_PREMAKE/gmake/libPGTATestCommon_x64.a libPGTATestCommon_x64.a"
popd

echo "--Copying PGTA Binaries--"

runCommand "cp -rf $PGTA_PREMAKE/gmake/libPGTALib.so $BUILD/"
runCommand "cp -rf $PGTA_PREMAKE/gmake/libSDL2.so $BUILD/"
runCommand "cp -rf $PGTA_PREMAKE/gmake/libSDL2-2.0.so.0 $BUILD/"
runCommand "cp -rf $PGTA_PREMAKE/gmake/libSDL2-2.0.so.0.4.0 $BUILD/"

echo "--Building PGTA Composer Application--"

runCommand 'qmake appsuite.pro'
runCommand 'make clean'
runCommand 'make'

echo "--Copying libPGTA--"
runCommand "cp -rf $BUILD/libPGTALib.so $BIN/"

echo "--Generate Run Script--"
pushd $BIN/
echo "#!/bin/sh\nexport LD_LIBRARY_PATH=.\n./PGTAComposer\n" > run.sh
chmod +x run.sh
popd
