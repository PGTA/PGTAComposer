-- Procedurally Generated Transitional Audio - Composer Dependencies Build --

function run_include(script, rel_dir)
    local external_dir = path.getabsolute("../external")
    local repo_dir = path.join(external_dir, rel_dir)
    local script_full = external_dir.."/build-tools/premake_scripts/"..script
    local output_dir = path.getabsolute("../premake/".._ACTION)
    assert(loadfile(script_full))(repo_dir, output_dir)
end

solution "Externals"

    location(_ACTION)
    targetdir(_ACTION)

    language "C++"
    platforms { "x64", "x32" }
    configurations { "Debug", "Release" }
    flags
    {
        "Symbols",
        "StaticRuntime",
        "NoMinimalRebuild",
        "NoEditAndContinue",
        "MultiProcessorCompile"
    }

    filter "action:vs*"
        defines
        {
            "_CRT_SECURE_NO_WARNINGS",
            "_SCL_SECURE_NO_WARNINGS",
            "_CRT_NONSTDC_NO_DEPRECATE"
        }
    filter { "action:not vs*" }
        buildoptions "-fPIC"
    filter { "action:not vs*", "language:C++" }
        buildoptions "-std=c++1y"

    filter "Debug"
        defines { "DEBUG", "_DEBUG" }
    filter "Release"
        flags "LinkTimeOptimization"
        defines "NDEBUG"
        optimize "Full"
    filter {}

    group "Externals"
        run_include("flatbuffers.lua", "flatbuffers")
    group "Externals"
        run_include("flatc.lua", "flatbuffers")
    group ""
