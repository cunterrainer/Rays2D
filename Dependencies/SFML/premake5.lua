project "SFML"
    kind "StaticLib"
    language "C++"

    defines {
        "SFML_STATIC",
        "UNICODE"
    }

    files {
        "**.cpp",
        "**.hpp",
        "**.h"
    }

    removefiles {
        "examples/**",
        "**/Unix/**",
        "**/OSX/**",
        "**/OpenBSD/**",
        "**/Android/**",
        "**/iOS/**",
        "**/FreeBSD/**",
        "**/EGLCheck.cpp",
        "**/EGLCheck.hpp",
        "**/EGLContext.cpp",
        "**/EGLContext.hpp",
        "**/MainAndroid.cpp",
        "**/SFMLActivity.cpp"
    }

    includedirs {
        "include",
        "src",
        "extlibs/headers",
        "extlibs/headers/AL",
        "extlibs/headers/freetype2",
        "extlibs/headers/stb_image"
    }