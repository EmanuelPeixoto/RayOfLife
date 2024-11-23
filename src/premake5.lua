workspace "Ray-of-Life"
    configurations { "Debug", "Release" }

project "Ray-of-Life"
    kind "WindowedApp"
    language "C"
    files { "main.c" }

    links { "raylib", "m" }

    filter "configurations:Debug"
        symbols "On"
    
    filter "configurations:Release"
        optimize "On"

project "Test-of-Life"
    kind "WindowedApp"
    language "C"
    files { "example.c" }

    links { "raylib", "m" }

    filter "configurations:Debug"
        symbols "On"
    
    filter "configurations:Release"
        optimize "On"

project "Grid-Test"
    kind "WindowedApp"
    language "C"
    files { "grid_test.c" }

    links { "raylib", "m" }

    filter "configurations:Debug"
        symbols "On"
    
    filter "configurations:Release"
        optimize "On"

