#include "FunkinPCH.h"
#include "ImGuiRenderer.h"
#include "ImGui/ImGuiFonts.h"
#include "ImGui/ImGuiThemes.h"
#include "Misc/Paths.h"
#include "VulkanRHI/VulkanImGuiRenderer.h"

#include <imgui.h>

void IImGuiRenderer::CreateContext()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    
    ImGuiIO& ImGuiConfig = ImGui::GetIO();
    ImGuiConfig.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGuiConfig.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGuiConfig.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    ImGuiConfig.ConfigWindowsMoveFromTitleBarOnly = true;
    
    // Setup fonts
    const std::filesystem::path& AssetsDirectory = CPaths::GetAssetsDirectory();
    
    CImGuiFonts::Add({
        .Name = "Inter",
        .Filepath = AssetsDirectory.string() + "/Fonts/Inter_28pt-Regular.ttf",
        .Size = 17.0f
    }, true);
    
    // Setup styling/theming
    // TODO: (Ayydxn) Apply either the dark or light theme based on a config setting.
    CImGuiThemes::ApplyDarkModeTheme();
    
    ImGuiStyle& Style = ImGui::GetStyle();
    if (ImGuiConfig.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        Style.WindowRounding = 0.0f;
        Style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }
    
    LOG_DEBUG_TAG("ImGui", "Initialized ImGui v{}", IMGUI_VERSION);
}

void IImGuiRenderer::DestroyContext()
{
    ImGui::DestroyContext();
}

IImGuiRenderer* CreateImGuiRenderer(ERHIBackend RHIBackend, IRHIContext& RHIContext)
{
    switch (RHIBackend)
    {
        case ERHIBackend::OpenGL: verifyFunkinf(false, "Failed to create ImGui renderer! OpenGL isn't supported!") break;
        
        case ERHIBackend::Vulkan:
        {
            auto& VulkanContext = dynamic_cast<CVulkanContext&>(RHIContext);
            
            return new CVulkanImGuiRenderer(VulkanContext);
        }
        
        case ERHIBackend::Direct3D11: verifyFunkinf(false, "Failed to create ImGui renderer! DirectX 11 isn't supported!") break;
        case ERHIBackend::Direct3D12: verifyFunkinf(false, "Failed to create ImGui renderer! DirectX 12 isn't supported!") break;
        case ERHIBackend::Metal: verifyFunkinf(false, "Failed to create ImGui renderer! Metal isn't supported!") break;
    }
    
    verifyFunkinf(false, "Failed to create ImGui renderer! An unknown/unsupported RHI backend was requested!")
    return nullptr;
}
