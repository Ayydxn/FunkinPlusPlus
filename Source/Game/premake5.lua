project "Game"
    kind "ConsoleApp"
    language "C++"
    
    pchsource "%{SourceDir}/Game/FunkinPCH.cpp"
    pchheader "FunkinPCH.h"
    
    IncludeDependencies({ "spdlog" })

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
        'PROJECT_ROOT="%{wks.location}"'
    }

    filter "configurations:Distribution"
        kind "WindowedApp"

    filter "system:windows"
        entrypoint "WinMainCRTStartup"
        
        targetname "FunkinPlusPlus-Win64-%{cfg.buildcfg}"
