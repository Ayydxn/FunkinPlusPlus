project "tracy"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "Off"

	files
	{
		"public/client/**.h",
		"public/client/**.hpp",
		"public/client/**.cpp",

		"public/common/**.h",
		"public/common/**.hpp",
		"public/common/**.cpp",

		"public/tracy/**.h",
		"public/tracy/**.hpp",
		"public/tracy/**.cpp",

		"public/libbacktrace/alloc.cpp",
		"public/libbacktrace/sort.cpp",
		"public/libbacktrace/state.cpp"
	}

	includedirs
	{
        "public/"
    }
    
    removefiles
    {
        "public/client/TracyRocprof.cpp"
    }

	filter "system:windows"
		systemversion "latest"
		
		targetdir("%{BinariesDir}/ThirdParty/Win64")
		objdir("%{IntermediateDir}/Win64/%{prj.name}/%{cfg.buildcfg}")

	filter "system:linux"
	    targetdir("%{BinariesDir}/Linux")
	    objdir("%{IntermediateDir}/Linux/%{prj.name}/%{cfg.buildcfg}")
                
		files
		{
			"public/libbacktrace/posix.cpp",
			"public/libbacktrace/mmapio.cpp",
			"public/libbacktrace/macho.cpp",
			"public/libbacktrace/fileline.cpp",
			"public/libbacktrace/elf.cpp",
			"public/libbacktrace/dwarf.cpp",
		}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"
		
		defines
		{
		    "TRACY_ENABLE",
		    "TRACY_ON_DEMAND"
		}
		
	filter "configurations:Development"
	    runtime "Release"
	    symbols "On"
	    optimize "Speed"
	    
	    defines
	    {
	        "TRACY_ENABLE",
	        "TRACY_ON_DEMAND"
	    }

	filter "configurations:Release"
		runtime "Release"
		symbols "On"
        optimize "Speed"

		defines
		{
			"TRACY_ENABLE",
			"TRACY_ON_DEMAND"
		}

	filter "configurations:Distribution"
		runtime "Release"
		symbols "Off"
        optimize "Speed"
