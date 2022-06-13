project "RayTracing"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"
    flags "FatalWarnings"

    defines "SFML_STATIC"

    -- gcc* clang* msc*
    filter "toolset:msc*"
        warnings "Everything"
        externalwarnings "Default"
        disablewarnings { 
            "4820", -- disable warning C4820: 'added padding'
            "4626", -- C6264 assignment operator was deleted
            "5027", -- C5027 move assignment operator was deleted
            "5045", -- C5045 Spectre mitigation
            "4710", -- C4710 function not inlined
            "4711", -- C4711 function 'function' selected for automatic inline expansion
        }
        buildoptions { "/sdl" }

    filter { "toolset:gcc* or toolset:clang*" }
        enablewarnings {
            "pedantic",
            "cast-align",
            "cast-qual",
            "ctor-dtor-privacy",
            "disabled-optimization",
            "format=2",
            "init-self",
            "missing-declarations",
            "missing-include-dirs",
            "old-style-cast",
            "overloaded-virtual",
            "redundant-decls",
            "shadow",
            "sign-conversion",
            "sign-promo",
            "strict-overflow=5",
            "switch-default",
            "undef",
            "uninitialized",
            "unreachable-code",
            "unused",
            "alloca",
            "conversion",
            "deprecated",
            "format-security",
            "null-dereference",
            "stack-protector",
            "vla",
            "shift-overflow"
        }

    filter "toolset:gcc*"
        warnings "Extra"
        externalwarnings "Off"
        linkgroups "on" -- activate position independent linking
        enablewarnings {
            "noexcept",
            "strict-null-sentinel",
            "array-bounds=2",
            "duplicated-branches",
            "duplicated-cond",
            "logical-op",
            "arith-conversion",
            "stringop-overflow=4",
            "implicit-fallthrough=3",
            "trampolines"
        }

    filter "toolset:clang*"
        warnings "Extra"
        externalwarnings "Everything"
        enablewarnings {
            "array-bounds",
            "long-long",
            "implicit-fallthrough", 
        }
    filter {}

    files {
        "**.cpp",
        "**.h"
    }

    includedirs {
        SfmlDir .. "/include"
    }

    externalincludedirs {
        SfmlDir .. "/include"
    }

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
        floatingpoint "default"

    filter { "configurations:Release" }
        kind "WindowedApp"
        floatingpoint "fast"