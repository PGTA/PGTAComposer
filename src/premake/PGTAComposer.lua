
function run_include(script, rel_dir)
    local external_dir = path.getabsolute("../external")
    local repo_dir = path.join(external_dir, rel_dir)
    local script_full = external_dir.."/build-tools/premake_scripts/"..script
    local output_dir = path.getabsolute("../premake/".._ACTION)
    assert(loadfile(script_full))(repo_dir, output_dir)
end

include "qt.lua"
local qt = premake.extensions.qt

solution "PGTAComposer"
    location(_ACTION)
    targetdir(_ACTION)
    startproject "PGTAComposer"

    language "C++"
    platforms "x64"
    configurations { "Debug", "Release" }
    flags
    {
        "Symbols",
        "NoMinimalRebuild",
        "NoEditAndContinue",
        "MultiProcessorCompile"
    }
    warnings "Extra"

    filter "action:vs*"
        defines
        {
            "_CRT_SECURE_NO_WARNINGS",
            "_SCL_SECURE_NO_WARNINGS",
            "_CRT_NONSTDC_NO_DEPRECATE"
        }
    filter { "action:not vs*", "language:C++" }
        buildoptions "-std=c++1y"
    filter { "action:not vs*", "language:C" }
        buildoptions "-std=c11"

    filter "Debug"
        defines { "DEBUG", "_DEBUG" }
    filter "Release"
        flags "LinkTimeOptimization"
        defines "NDEBUG"
        optimize "Full"
    filter {}
    
    project "PGTAComposer"
        debugdir "../.."
        debugcommand "%{cfg.buildtarget.directory}../../../bin/%{cfg.buildtarget.name}"
        kind "WindowedApp"
        qt.enable()
        qtpath "C:/Qt/5.4/msvc2013_64/"
        qtprefix "Qt5"
        qtmodules
        {
            "core",
            "gui",
            "widgets"
        }
        configuration "Debug"
            qtsuffix "d"
        configuration {}
        includedirs
        {
            "../external/PGTA/src/PGTA/",
            "../external/PGTA/src/PGTA/public",
            "../external/PGTA/src/external/flatbuffers/include/",
            "../external/PGTA/src/tests/PGTATestCommon/"
        }
        libdirs
        {
            "../external/PGTA/src/premake/vs2013"
        }
        links
        {
            "_FlatBuffers",
            "PGTALib",
            "PGTATestCommon_x64",
            "SDL2"
        }
        files
        {
            "../pgta_composer/**.h",
            "../pgta_composer/**.cpp",
            "../pgta_composer/**.qrc"
        }
        configuration "Debug"
            postbuildcommands
            {
                "{MKDIR} ../../../bin",
                "{COPY} %{cfg.buildtarget.name} ../../../bin/",
                "{COPY} %{cfg.qtpath}/bin/icudt53.dll ../../../bin",
                "{COPY} %{cfg.qtpath}/bin/icuin53.dll ../../../bin",
                "{COPY} %{cfg.qtpath}/bin/icuuc53.dll ../../../bin",
                "{COPY} %{cfg.qtpath}/bin/Qt5Cored.dll ../../../bin",
                "{COPY} %{cfg.qtpath}/bin/Qt5Guid.dll ../../../bin",
                "{COPY} %{cfg.qtpath}/bin/Qt5Widgetsd.dll ../../../bin",
                "{COPY} ../../external/PGTA/src/premake/vs2013/SDL2.dll ../../../bin/",
                "{COPY} ../../external/PGTA/src/premake/vs2013/PGTALib.dll ../../../bin",
            }

        configuration "Release"
            postbuildcommands
            {
                "{MKDIR} ../../../bin",
                "{COPY} %{cfg.buildtarget.name} ../../../bin/",
                "{COPY} %{cfg.qtpath}/bin/icudt53.dll ../../../bin",
                "{COPY} %{cfg.qtpath}/bin/icuin53.dll ../../../bin",
                "{COPY} %{cfg.qtpath}/bin/icuuc53.dll ../../../bin",
                "{COPY} %{cfg.qtpath}/bin/Qt5Core.dll ../../../bin",
                "{COPY} %{cfg.qtpath}/bin/Qt5Gui.dll ../../../bin",
                "{COPY} %{cfg.qtpath}/bin/Qt5Widgets.dll ../../../bin",
                "{COPY} ../../external/PGTA/src/premake/vs2013/SDL2.dll ../../../bin/",
                "{COPY} ../../external/PGTA/src/premake/vs2013/PGTALib.dll ../../../bin",
            }
        configuration {}
