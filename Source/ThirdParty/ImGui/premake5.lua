project "ImGui"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
    staticruntime "off"

	files
	{
		"imgui.h",
		"imgui.cpp",
		"imgui_draw.cpp",
		"imgui_internal.h",
		"imgui_tables.cpp",
		"imgui_widgets.cpp",
		"imstb_rectpack.h",
		"imstb_textedit.h",
		"imstb_truetype.h",
		"imgui_demo.cpp",
		"imconfig.h",
		
		"backends/imgui_impl_sdl3.cpp",
		"backends/imgui_impl_sdl3.h",
		"backends/imgui_impl_vulkan.cpp",
		"backends/imgui_impl_vulkan.h"
	}

    includedirs
    {
        "%{prj.location}",
        
        "%{wks.location}/Source/ThirdParty/SDL3/Include",
        "%{wks.location}/Source/ThirdParty/Vulkan/Include"
    }
    
    defines
    {
        "IMGUI_IMPL_VULKAN_NO_PROTOTYPES"
    }

	filter "system:windows"
		systemversion "latest"
		
		files
		{
            "backends/imgui_impl_dx11.cpp",
            "backends/imgui_impl_dx11.h",
            "backends/imgui_impl_dx12.cpp",
            "backends/imgui_impl_dx12.h"
        }
    
    filter "system:macosx"
        files
        {
            "backends/imgui_impl_metal.mm",
            "backends/imgui_impl_osx.mm",
            "backends/imgui_impl_osx.h",
        }

	filter "system:linux"
		pic "On"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"

    filter "configurations:Distribution"
		runtime "Release"
		optimize "On"
        symbols "Off"