project "RayTracing"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"
    warnings "High"

    defines "SFML_STATIC"

    files {
        "**.cpp",
        "**.h"
    }

    includedirs {
        SfmlDir .. "/include"
    }

    -- activate position independent linking
    linkgroups "on"

    links {
        "winmm",
        "flac",
        "freetype",
        "ogg",
        "openal32",
        "opengl32",
        "vorbis",
        "vorbisenc",
        "vorbisfile",
        "gdi32",
        "SFML",
        "user32",
        "advapi32"
    }

    filter { "platforms:x64" }
        libdirs {
            SfmlDir .. "/extlibs/libs-msvc-universal/x64"
        }

    filter { "platforms:x86" }
        libdirs {
            SfmlDir .. "/extlibs/libs-msvc-universal/x86"
        }

    filter { "configurations:Debug" }
        kind "ConsoleApp"

    filter { "configurations:Release" }
        kind "WindowedApp"