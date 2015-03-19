
set msbuild="%VS120COMNTOOLS%/../../../MSBuild/12.0/Bin/MSBuild"
set qmake="C:/Qt/5.4/msvc2013_64/bin/qmake"

pushd "%VS120COMNTOOLS%/../../VC/"
    call vcvarsall.bat amd64
popd

echo "--Building PGTA Composer Dependencies--"

pushd "../external/PGTA/src/premake"
    :: echo | skips the pause in vs2013.bat
    echo | call vs2013.bat
    pushd "vs2013"
        call %msbuild% /p:Configuration=Debug /p:Platform=x64 PGTA.sln
    popd
popd

pushd "../external/qt-solutions-mirror/qtpropertybrowser"
    call configure.bat -library
    call %qmake%
    call nmake
popd

echo "--Building PGTA Composer Application--"

call "premake5" clean --file=PGTAComposer.lua
call "premake5" vs2013 --file=PGTAComposer.lua

pause
