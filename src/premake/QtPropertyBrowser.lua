
include "qt.lua"
local qt = premake.extensions.qt

project "QtPropertyBrowser"
    kind "SharedLib"
    qt.enable()
    qtpath "C:/Qt/5.4/msvc2013_64/"
    qtprefix "Qt5"
    qtmodules
    {
        "core",
        "gui",
        "widgets"
    }
    filter "Debug"
        qtsuffix "d"
    filter {}
    defines
    {
        "QT_DISABLE_DEPRECATED_BEFORE=0"
    }
    files
    {
        "../external/qt-solutions-mirror/qtpropertybrowser/src/**.h",
        "../external/qt-solutions-mirror/qtpropertybrowser/src/**.cpp",
        "../external/qt-solutions-mirror/qtpropertybrowser/src/**.ui",
        "../external/qt-solutions-mirror/qtpropertybrowser/src/**.qrc"
    }
