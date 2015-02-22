echo "--Setting up Build Environment--"

set msbuild=C:\Windows\Microsoft.NET\Framework\v4.0.30319\MSBuild.exe
set qmake=C:\Qt\5.3\msvc2013\bin\qmake.exe
set jom=C:\Qt\Tools\QtCreator\bin\jom.exe

call "c:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x86

echo "--Building PGTA Composer Dependencies--"

"../external/build-tools/bin/premake/premake5" clean --file=externals.lua
"../external/build-tools/bin/premake/premake5" vs2013 --file=externals.lua

pushd vs2013

%msbuild% /nologo /p:Configuration=Debug /p:Platform=Win32 /p:VisualStudioVersion=13.0 Externals.sln

popd

echo "--Building PGTA Composer Applicatoin--"

%qmake% appsuite.pro

%jom%

pause
