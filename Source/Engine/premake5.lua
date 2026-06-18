project "Engine"
    kind "SharedLib"
    language "C++"
    
    pchsource "%{SourceDir}/Engine/FunkinPCH.cpp"
    pchheader "FunkinPCH.h"

    files
    {
        "%{SourceDir}/Engine/**.cpp",
        "%{SourceDir}/Engine/**.h"
    }

    includedirs
    {
        "%{SourceDir}/Engine"
    }

    defines
    {
        "FUNKIN_EXPORT_LIBRARY"
    }
    
    filter "system:windows"
        targetname "FunkinEngine-Win64-%{cfg.buildcfg}"
