workspace "RayTracing"
    platforms { "x64", "x86" }
    configurations {
        "Debug",
        "Release"
    }
    startproject "RayTracing"

outputdir = "/BIN/%{cfg.buildcfg}/%{cfg.architecture}/"
-- get current working directory
cwd = os.getcwd()
SfmlDir = cwd .. "/Dependencies/SFML"

targetdir(cwd .. outputdir .. "%{prj.name}/bin")
objdir(cwd .. outputdir .. "%{prj.name}/bin-int")

filter { "platforms:x64" }
    architecture "x64"
filter { "platforms:x86" }
    architecture "x86"


filter { "configurations:Debug" }
    defines "DEBUG"
filter { "configurations:Release" }
    defines "NDEBUG"


filter { "configurations:Debug" }
    runtime "Debug"
    symbols "on"
filter { "configurations:Release" }
    runtime "Release"
    symbols "off"
    optimize "Speed"

filter {}

-- only for visual studio
flags {
    "MultiProcessorCompile"
}
staticruntime "on"
removeunreferencedcodedata "on"

include "RayTracing"
include "Dependencies/SFML"