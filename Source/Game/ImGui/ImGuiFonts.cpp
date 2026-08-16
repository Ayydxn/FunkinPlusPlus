#include "FunkinPCH.h"
#include "ImGuiFonts.h"
#include "Misc/Paths.h"

#include <imgui.h>

namespace
{
    std::unordered_map<std::string, ImFont*> LoadedFonts;
}

void CImGuiFonts::Add(const FFontConfiguration& Configuration, bool bIsDefault)
{
    if (LoadedFonts.contains(Configuration.Name))
    {
        LOG_WARN_TAG("ImGui", "Tried to add font with name '{}' but a font with that name is already present!", Configuration.Name);
        return;
    }
    
    ImGuiIO& ImGuiConfig = ImGui::GetIO();
    
    ImFont* Font = ImGuiConfig.Fonts->AddFontFromFileTTF(Configuration.Filepath.c_str(), Configuration.Size);
    LoadedFonts.emplace(Configuration.Name, Font);
    
    if (bIsDefault)
        ImGuiConfig.FontDefault = Font;
}
