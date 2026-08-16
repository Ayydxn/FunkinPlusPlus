#include "FunkinPCH.h"
#include "ImGuiCommon.h"
#include "ImGuiFonts.h"
#include "ImGuiThemes.h"
#include "Misc/Paths.h"

#include <imgui.h>

void CImGuiCommon::CreateContext()
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

void CImGuiCommon::DestroyContext()
{
    ImGui::DestroyContext();
}
