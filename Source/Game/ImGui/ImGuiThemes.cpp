#include "FunkinPCH.h"
#include "ImGuiThemes.h"

void CImGuiThemes::ApplyDarkModeTheme()
{
    ImGuiStyle& Style = ImGui::GetStyle();

    // Rounding
    Style.WindowRounding = 0.0f;
    Style.ChildRounding = 0.0f;
    Style.FrameRounding = 2.0f;
    Style.GrabRounding = 2.0f;
    Style.PopupRounding = 2.0f;
    Style.ScrollbarRounding = 2.0f;
    Style.TabRounding = 2.0f;

    // Borders
    Style.WindowBorderSize = 1.0f;
    Style.ChildBorderSize = 1.0f;
    Style.FrameBorderSize = 0.0f;
    Style.PopupBorderSize = 1.0f;
    Style.TabBorderSize = 0.0f;

    // Spacing
    Style.WindowPadding = ImVec2(8.0f, 8.0f);
    Style.FramePadding = ImVec2(6.0f, 3.0f);
    Style.ItemSpacing = ImVec2(8.0f, 4.0f);
    Style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
    Style.IndentSpacing = 16.0f;
    Style.ScrollbarSize = 10.0f;
    Style.GrabMinSize = 8.0f;
    Style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
    Style.SeparatorTextPadding = ImVec2(20.0f, 3.0f);

    // Colors
    ImVec4* Colors = Style.Colors;
    Colors[ImGuiCol_Text] = FDarkThemeColors::Text;
    Colors[ImGuiCol_TextDisabled] = FDarkThemeColors::TextDisabled;
    Colors[ImGuiCol_WindowBg] = FDarkThemeColors::Background;
    Colors[ImGuiCol_ChildBg] = FDarkThemeColors::BackgroundDark;
    Colors[ImGuiCol_PopupBg] = FDarkThemeColors::BackgroundPopup;
    Colors[ImGuiCol_Border] = FDarkThemeColors::Border;
    Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    Colors[ImGuiCol_FrameBg] = FDarkThemeColors::FrameBg;
    Colors[ImGuiCol_FrameBgHovered] = FDarkThemeColors::FrameBgHovered;
    Colors[ImGuiCol_FrameBgActive] = FDarkThemeColors::FrameBgActive;
    Colors[ImGuiCol_TitleBg] = FDarkThemeColors::TitleBar;
    Colors[ImGuiCol_TitleBgActive] = FDarkThemeColors::TitleBarActive;
    Colors[ImGuiCol_TitleBgCollapsed] = FDarkThemeColors::TitleBar;
    Colors[ImGuiCol_MenuBarBg] = FDarkThemeColors::BackgroundDark;
    Colors[ImGuiCol_ScrollbarBg] = FDarkThemeColors::ScrollbarBg;
    Colors[ImGuiCol_ScrollbarGrab] = FDarkThemeColors::ScrollbarGrab;
    Colors[ImGuiCol_ScrollbarGrabHovered] = FDarkThemeColors::ScrollbarGrabHover;
    Colors[ImGuiCol_ScrollbarGrabActive] = FDarkThemeColors::ScrollbarGrabActive;
    Colors[ImGuiCol_CheckMark] = FDarkThemeColors::CheckMark;
    Colors[ImGuiCol_SliderGrab] = FDarkThemeColors::SliderGrab;
    Colors[ImGuiCol_SliderGrabActive] = FDarkThemeColors::SliderGrabActive;
    Colors[ImGuiCol_Button] = FDarkThemeColors::Button;
    Colors[ImGuiCol_ButtonHovered] = FDarkThemeColors::ButtonHovered;
    Colors[ImGuiCol_ButtonActive] = FDarkThemeColors::ButtonActive;
    Colors[ImGuiCol_Header] = FDarkThemeColors::Header;
    Colors[ImGuiCol_HeaderHovered] = FDarkThemeColors::HeaderHovered;
    Colors[ImGuiCol_HeaderActive] = FDarkThemeColors::HeaderActive;
    Colors[ImGuiCol_Separator] = FDarkThemeColors::Separator;
    Colors[ImGuiCol_SeparatorHovered] = FDarkThemeColors::Accent;
    Colors[ImGuiCol_SeparatorActive] = FDarkThemeColors::AccentActive;
    Colors[ImGuiCol_ResizeGrip] = FDarkThemeColors::ResizeGrip;
    Colors[ImGuiCol_ResizeGripHovered] = FDarkThemeColors::ResizeGripHovered;
    Colors[ImGuiCol_ResizeGripActive] = FDarkThemeColors::ResizeGripActive;
    Colors[ImGuiCol_Tab] = FDarkThemeColors::Tab;
    Colors[ImGuiCol_TabHovered] = FDarkThemeColors::TabHovered;
    Colors[ImGuiCol_TabActive] = FDarkThemeColors::TabActive;
    Colors[ImGuiCol_TabUnfocused] = FDarkThemeColors::TabUnfocused;
    Colors[ImGuiCol_TabUnfocusedActive] = FDarkThemeColors::TabUnfocusedActive;
    Colors[ImGuiCol_TabSelectedOverline] = FDarkThemeColors::TabActive;
    Colors[ImGuiCol_DockingPreview] = FDarkThemeColors::Accent;
    Colors[ImGuiCol_DockingEmptyBg] = FDarkThemeColors::BackgroundDark;
    Colors[ImGuiCol_PlotLines] = FDarkThemeColors::Accent;
    Colors[ImGuiCol_PlotLinesHovered] = FDarkThemeColors::AccentHovered;
    Colors[ImGuiCol_PlotHistogram] = FDarkThemeColors::Accent;
    Colors[ImGuiCol_PlotHistogramHovered] = FDarkThemeColors::AccentHovered;
    Colors[ImGuiCol_TableHeaderBg] = FDarkThemeColors::BackgroundDark;
    Colors[ImGuiCol_TableBorderStrong] = FDarkThemeColors::Border;
    Colors[ImGuiCol_TableBorderLight] = FDarkThemeColors::Separator;
    Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.03f);
    Colors[ImGuiCol_TextSelectedBg] = ImVec4(FDarkThemeColors::Accent.x, FDarkThemeColors::Accent.y, FDarkThemeColors::Accent.z, 0.35f);
    Colors[ImGuiCol_DragDropTarget] = FDarkThemeColors::Accent;
    Colors[ImGuiCol_NavHighlight] = FDarkThemeColors::Accent;
    Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.70f);
    Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.8f, 0.8f, 0.8f, 0.20f);
    Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.1f, 0.1f, 0.1f, 0.50f);
}

void CImGuiThemes::ApplyLightModeTheme()
{
    ImGuiStyle& Style = ImGui::GetStyle();

    // Rounding
    Style.WindowRounding = 0.0f;
    Style.ChildRounding = 0.0f;
    Style.FrameRounding = 2.0f;
    Style.GrabRounding = 2.0f;
    Style.PopupRounding = 2.0f;
    Style.ScrollbarRounding = 2.0f;
    Style.TabRounding = 2.0f;

    // Borders
    Style.WindowBorderSize = 1.0f;
    Style.ChildBorderSize = 1.0f;
    Style.FrameBorderSize = 0.0f;
    Style.PopupBorderSize = 1.0f;
    Style.TabBorderSize = 0.0f;

    // Spacing
    Style.WindowPadding = ImVec2(8.0f, 8.0f);
    Style.FramePadding = ImVec2(6.0f, 3.0f);
    Style.ItemSpacing = ImVec2(8.0f, 4.0f);
    Style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
    Style.IndentSpacing = 16.0f;
    Style.ScrollbarSize = 10.0f;
    Style.GrabMinSize = 8.0f;
    Style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
    Style.SeparatorTextPadding = ImVec2(20.0f, 3.0f);

    // Colors
    ImVec4* Colors = Style.Colors;
    Colors[ImGuiCol_Text] = FLightThemeColors::Text;
    Colors[ImGuiCol_TextDisabled] = FLightThemeColors::TextDisabled;
    Colors[ImGuiCol_WindowBg] = FLightThemeColors::Background;
    Colors[ImGuiCol_ChildBg] = FLightThemeColors::BackgroundDark;
    Colors[ImGuiCol_PopupBg] = FLightThemeColors::BackgroundPopup;
    Colors[ImGuiCol_Border] = FLightThemeColors::Border;
    Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    Colors[ImGuiCol_FrameBg] = FLightThemeColors::FrameBg;
    Colors[ImGuiCol_FrameBgHovered] = FLightThemeColors::FrameBgHovered;
    Colors[ImGuiCol_FrameBgActive] = FLightThemeColors::FrameBgActive;
    Colors[ImGuiCol_TitleBg] = FLightThemeColors::TitleBar;
    Colors[ImGuiCol_TitleBgActive] = FLightThemeColors::TitleBarActive;
    Colors[ImGuiCol_TitleBgCollapsed] = FLightThemeColors::TitleBar;
    Colors[ImGuiCol_MenuBarBg] = FLightThemeColors::BackgroundDark;
    Colors[ImGuiCol_ScrollbarBg] = FLightThemeColors::ScrollbarBg;
    Colors[ImGuiCol_ScrollbarGrab] = FLightThemeColors::ScrollbarGrab;
    Colors[ImGuiCol_ScrollbarGrabHovered] = FLightThemeColors::ScrollbarGrabHover;
    Colors[ImGuiCol_ScrollbarGrabActive] = FLightThemeColors::ScrollbarGrabActive;
    Colors[ImGuiCol_CheckMark] = FLightThemeColors::CheckMark;
    Colors[ImGuiCol_SliderGrab] = FLightThemeColors::SliderGrab;
    Colors[ImGuiCol_SliderGrabActive] = FLightThemeColors::SliderGrabActive;
    Colors[ImGuiCol_Button] = FLightThemeColors::Button;
    Colors[ImGuiCol_ButtonHovered] = FLightThemeColors::ButtonHovered;
    Colors[ImGuiCol_ButtonActive] = FLightThemeColors::ButtonActive;
    Colors[ImGuiCol_Header] = FLightThemeColors::Header;
    Colors[ImGuiCol_HeaderHovered] = FLightThemeColors::HeaderHovered;
    Colors[ImGuiCol_HeaderActive] = FLightThemeColors::HeaderActive;
    Colors[ImGuiCol_Separator] = FLightThemeColors::Separator;
    Colors[ImGuiCol_SeparatorHovered] = FLightThemeColors::Accent;
    Colors[ImGuiCol_SeparatorActive] = FLightThemeColors::AccentActive;
    Colors[ImGuiCol_ResizeGrip] = FLightThemeColors::ResizeGrip;
    Colors[ImGuiCol_ResizeGripHovered] = FLightThemeColors::ResizeGripHovered;
    Colors[ImGuiCol_ResizeGripActive] = FLightThemeColors::ResizeGripActive;
    Colors[ImGuiCol_Tab] = FLightThemeColors::Tab;
    Colors[ImGuiCol_TabHovered] = FLightThemeColors::TabHovered;
    Colors[ImGuiCol_TabActive] = FLightThemeColors::TabActive;
    Colors[ImGuiCol_TabUnfocused] = FLightThemeColors::TabUnfocused;
    Colors[ImGuiCol_TabUnfocusedActive] = FLightThemeColors::TabUnfocusedActive;
    Colors[ImGuiCol_TabSelectedOverline] = FLightThemeColors::TabActive;
    Colors[ImGuiCol_DockingPreview] = FLightThemeColors::Accent;
    Colors[ImGuiCol_DockingEmptyBg] = FLightThemeColors::BackgroundDark;
    Colors[ImGuiCol_PlotLines] = FLightThemeColors::Accent;
    Colors[ImGuiCol_PlotLinesHovered] = FLightThemeColors::AccentHovered;
    Colors[ImGuiCol_PlotHistogram] = FLightThemeColors::Accent;
    Colors[ImGuiCol_PlotHistogramHovered] = FLightThemeColors::AccentHovered;
    Colors[ImGuiCol_TableHeaderBg] = FLightThemeColors::BackgroundDark;
    Colors[ImGuiCol_TableBorderStrong] = FLightThemeColors::Border;
    Colors[ImGuiCol_TableBorderLight] = FLightThemeColors::Separator;
    Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.03f);
    Colors[ImGuiCol_TextSelectedBg] = ImVec4(FLightThemeColors::Accent.x, FLightThemeColors::Accent.y, FLightThemeColors::Accent.z, 0.35f);
    Colors[ImGuiCol_DragDropTarget] = FLightThemeColors::Accent;
    Colors[ImGuiCol_NavHighlight] = FLightThemeColors::Accent;
    Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.70f);
    Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.8f, 0.8f, 0.8f, 0.20f);
    Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.1f, 0.1f, 0.1f, 0.50f);
}
