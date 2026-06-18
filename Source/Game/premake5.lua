project "Game"
    kind "ConsoleApp"
    language "C++"

    files
    {
        "%{SourceDir}/Game/**.cpp",
        "%{SourceDir}/Game/**.h"
    }

    includedirs
    {
        "%{SourceDir}/Game",
        
        "%{SourceDir}/Engine"
    }

    links
    {
        "Engine"
    }

    filter "configurations:Distribution"
        kind "WindowedApp"

    filter "system:windows"
        entrypoint "WinMainCRTStartup"
        
        targetname "FunkinPlusPlus-Win64-%{cfg.buildcfg}"
