include "Dependencies.lua"

workspace "Funkin++"
    architecture "x86_64"
    startproject "Game"
    conformancemode "Off"
    multiprocessorcompile "On"
    cppdialect "C++20"
    staticruntime "Off"

    RootDir = "%{wks.location}"
    SourceDir = "%{wks.location}/Source"
    ThirdPartyDir = "%{wks.location}/Source/ThirdParty"
    BinariesDir = "%{wks.location}/Binaries"
    IntermediateDir = "%{wks.location}/Intermediate"

    configurations
    {
        -- Maximum debugging capbilities. Optimizations are disabled and full debug symbols are generated for accurate line-by-line debugging.
        "Debug",

        -- Combines production-speed code optimization with full debug symbols, allowing you to trace real-world latency while keeping the debugger attachable.
        "Development",

        -- Maximizes code execution speeds and enables Link-Time Optimization across source files, but keeps debug symbols active.
        "Release",

        -- Optimizes code for absolute maximum runtime performance and minimum input latency.
        "Distribution"
    }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"
        optimize "Off"

        defines
        {
            "FUNKIN_BUILD_DEBUG"
        }

    filter "configurations:Development"
        runtime "Release"
        symbols "On"
        optimize "Speed"

        defines
        {
            "FUNKIN_BUILD_DEVELOPMENT"
        }

    filter "configurations:Release"
        runtime "Release"
        symbols "On"
        optimize "Speed"
        linktimeoptimization "On"

        defines
        {
            "FUNKIN_BUILD_RELEASE"
        }

    filter "configurations:Distribution"
        runtime "Release"
        symbols "Off"
        optimize "Speed"
        linktimeoptimization "On"
        omitframepointer "On"

         defines
         {
            "FUNKIN_BUILD_DISTRIBUTION"
         }

    filter "system:windows"
        systemversion "latest"
        
        targetdir("%{BinariesDir}/Win64")
        objdir("%{IntermediateDir}/Win64/%{prj.name}/%{cfg.buildcfg}")

        buildoptions
        {
            "/EHsc",
            "/Zc:preprocessor",
            "/Zc:__cplusplus"
        }

        defines
        {
            "FUNKIN_PLATFORM_WIN64",
            
            "WIN32_LEAN_AND_MEAN",
            "_WINDLL",
            "_CRT_SECURE_NO_WARNINGS",
            "NOMINMAX"
        }

    filter "system:macosx"
        targetdir("%{BinariesDir}/Mac")
        objdir("%{IntermediateDir}/Mac/%{prj.name}/%{cfg.buildcfg}")
                
        defines
        {
            "FUNKIN_PLATFORM_MAC"
        }
    
    filter "system:linux"
        targetdir("%{BinariesDir}/Linux")
        objdir("%{IntermediateDir}/Linux/%{prj.name}/%{cfg.buildcfg}")
        
        defines
        {
            "FUNKIN_PLATFORM_LINUX"
        }
    filter {}

group "Core"
    include "Source/Engine"
    include "Source/Game"
group ""
