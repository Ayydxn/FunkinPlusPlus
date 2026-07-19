project "Game"
    kind "ConsoleApp"
    language "C++"
    
    pchsource "%{SourceDir}/Game/FunkinPCH.cpp"
    pchheader "FunkinPCH.h"
    
    IncludeDependencies({ "spdlog", "SDL3", "tracy", "Vulkan", "Slang" })

    files
    {
        "%{SourceDir}/Game/**.cpp",
        "%{SourceDir}/Game/**.h"
    }

    includedirs
    {
        "%{SourceDir}/Game",
        "%{SourceDir}/Game/Core"
    }
    
    defines
    {
        'PROJECT_ROOT="%{wks.location}"',
        
        "SDL_MAIN_DEFINED"
    }
    
    filter "configurations:Debug or configurations:Development or configurations:Release"
        defines
        {
            "TRACY_ENABLE",
            "TRACY_ON_DEMAND"
        }

    filter "configurations:Distribution"
        kind "WindowedApp"

    filter "system:windows"
        entrypoint "WinMainCRTStartup"
        targetname "FunkinPlusPlus-Win64-%{cfg.buildcfg}"
        
        postbuildcommands
        {
            '{COPYFILE} "%{wks.location}/Source/ThirdParty/SDL3/Binaries/Win64/*.dll" "%{cfg.buildtarget.directory}"',
            '{COPYFILE} "%{wks.location}/Source/ThirdParty/Slang/Binaries/Win64/*.dll" "%{cfg.buildtarget.directory}"'
        }
