echo "--Setting up Build Environment--"

set msbuild="%VS120COMNTOOLS%\..\..\..\MSBuild\12.0\Bin\MSBuild"
set qmake="C:\Qt\5.3\msvc2013\bin\qmake"
set jom="C:\Qt\Tools\QtCreator\bin\jom"

call "%VS120COMNTOOLS%\..\..\VC\vcvarsall.bat" x86

echo "--Building PGTA Composer Dependencies--"

"../external/build-tools/bin/premake/premake5" clean --file=externals.lua
"../external/build-tools/bin/premake/premake5" vs2013 --file=externals.lua

pushd vs2013

call %msbuild% /nologo /p:Configuration=Release /p:Platform=Win32 /p:VisualStudioVersion=13.0 Externals.sln

popd

echo "--Building PGTA Composer Application--"

call %qmake% appsuite.pro

call %jom%

pause
