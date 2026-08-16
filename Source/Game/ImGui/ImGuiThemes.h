#pragma once

#include <imgui.h>

struct FDarkThemeColors
{
    // Backgrounds
    static constexpr ImVec4 Background = { 0.105f, 0.105f, 0.105f, 1.0f };
    static constexpr ImVec4 BackgroundDark = { 0.074f, 0.074f, 0.074f, 1.0f };
    static constexpr ImVec4 BackgroundPopup = { 0.133f, 0.133f, 0.133f, 1.0f };
    static constexpr ImVec4 TitleBar = { 0.063f, 0.063f, 0.063f, 1.0f };
    static constexpr ImVec4 TitleBarActive = { 0.078f, 0.078f, 0.078f, 1.0f };

    // Frames
    static constexpr ImVec4 FrameBg = { 0.149f, 0.149f, 0.149f, 1.0f };
    static constexpr ImVec4 FrameBgHovered = { 0.196f, 0.196f, 0.196f, 1.0f };
    static constexpr ImVec4 FrameBgActive = { 0.219f, 0.219f, 0.219f, 1.0f };

    // Buttons
    static constexpr ImVec4 Button = { 0.176f, 0.176f, 0.176f, 1.0f };
    static constexpr ImVec4 ButtonHovered = { 0.235f, 0.235f, 0.235f, 1.0f };
    static constexpr ImVec4 ButtonActive = { 0.294f, 0.294f, 0.294f, 1.0f };

    // Headers
    static constexpr ImVec4 Header = { 0.196f, 0.196f, 0.196f, 1.0f };
    static constexpr ImVec4 HeaderHovered = { 0.235f, 0.235f, 0.235f, 1.0f };
    static constexpr ImVec4 HeaderActive = { 0.274f, 0.274f, 0.274f, 1.0f };

    // Tabs
    static constexpr ImVec4 Tab = { 0.090f, 0.090f, 0.090f, 1.0f };
    static constexpr ImVec4 TabHovered = { 0.235f, 0.235f, 0.235f, 1.0f };
    static constexpr ImVec4 TabActive = { 0.176f, 0.176f, 0.176f, 1.0f };
    static constexpr ImVec4 TabUnfocused = { 0.090f, 0.090f, 0.090f, 1.0f };
    static constexpr ImVec4 TabUnfocusedActive = { 0.133f, 0.133f, 0.133f, 1.0f };

    // Accent
    static constexpr ImVec4 Accent = { 0.878f, 0.420f, 0.098f, 1.0f };
    static constexpr ImVec4 AccentHovered = { 0.941f, 0.494f, 0.165f, 1.0f };
    static constexpr ImVec4 AccentActive = { 0.780f, 0.353f, 0.063f, 1.0f };

    // Borders
    static constexpr ImVec4 Border = { 0.219f, 0.219f, 0.219f, 1.0f };
    static constexpr ImVec4 Separator = { 0.188f, 0.188f, 0.188f, 1.0f };

    // Text
    static constexpr ImVec4 Text = { 0.847f, 0.847f, 0.847f, 1.0f };
    static constexpr ImVec4 TextDisabled = { 0.439f, 0.439f, 0.439f, 1.0f };

    // Scrollbar
    static constexpr ImVec4 ScrollbarBg = { 0.086f, 0.086f, 0.086f, 1.0f };
    static constexpr ImVec4 ScrollbarGrab = { 0.239f, 0.239f, 0.239f, 1.0f };
    static constexpr ImVec4 ScrollbarGrabHover = { 0.294f, 0.294f, 0.294f, 1.0f };
    static constexpr ImVec4 ScrollbarGrabActive = { 0.352f, 0.352f, 0.352f, 1.0f };

    // Misc
    static constexpr ImVec4 CheckMark = { 0.878f, 0.420f, 0.098f, 1.0f };
    static constexpr ImVec4 SliderGrab = { 0.439f, 0.439f, 0.439f, 1.0f };
    static constexpr ImVec4 SliderGrabActive = { 0.878f, 0.420f, 0.098f, 1.0f };
    static constexpr ImVec4 ResizeGrip = { 0.878f, 0.420f, 0.098f, 0.25f };
    static constexpr ImVec4 ResizeGripHovered = { 0.878f, 0.420f, 0.098f, 0.67f };
    static constexpr ImVec4 ResizeGripActive = { 0.878f, 0.420f, 0.098f, 0.95f };
};

struct FLightThemeColors
{
    // Backgrounds
    static constexpr ImVec4 Background = { 0.941f, 0.941f, 0.941f, 1.0f };
    static constexpr ImVec4 BackgroundDark = { 0.882f, 0.882f, 0.882f, 1.0f };
    static constexpr ImVec4 BackgroundPopup = { 0.980f, 0.980f, 0.980f, 1.0f };
    static constexpr ImVec4 TitleBar = { 0.863f, 0.863f, 0.863f, 1.0f };
    static constexpr ImVec4 TitleBarActive = { 0.800f, 0.800f, 0.800f, 1.0f };

    // Frames
    static constexpr ImVec4 FrameBg = { 0.863f, 0.863f, 0.863f, 1.0f };
    static constexpr ImVec4 FrameBgHovered = { 0.800f, 0.800f, 0.800f, 1.0f };
    static constexpr ImVec4 FrameBgActive = { 0.737f, 0.737f, 0.737f, 1.0f };

    // Buttons
    static constexpr ImVec4 Button = { 0.843f, 0.843f, 0.843f, 1.0f };
    static constexpr ImVec4 ButtonHovered = { 0.765f, 0.765f, 0.765f, 1.0f };
    static constexpr ImVec4 ButtonActive = { 0.686f, 0.686f, 0.686f, 1.0f };

    // Headers
    static constexpr ImVec4 Header = { 0.835f, 0.835f, 0.835f, 1.0f };
    static constexpr ImVec4 HeaderHovered = { 0.765f, 0.765f, 0.765f, 1.0f };
    static constexpr ImVec4 HeaderActive = { 0.706f, 0.706f, 0.706f, 1.0f };

    // Tabs
    static constexpr ImVec4 Tab = { 0.835f, 0.835f, 0.835f, 1.0f };
    static constexpr ImVec4 TabHovered = { 0.745f, 0.745f, 0.745f, 1.0f };
    static constexpr ImVec4 TabActive = { 0.941f, 0.941f, 0.941f, 1.0f };
    static constexpr ImVec4 TabUnfocused = { 0.863f, 0.863f, 0.863f, 1.0f };
    static constexpr ImVec4 TabUnfocusedActive = { 0.902f, 0.902f, 0.902f, 1.0f };

    // Accent
    static constexpr ImVec4 Accent = { 0.820f, 0.360f, 0.050f, 1.0f };
    static constexpr ImVec4 AccentHovered = { 0.900f, 0.440f, 0.100f, 1.0f };
    static constexpr ImVec4 AccentActive = { 0.710f, 0.290f, 0.020f, 1.0f };

    // Borders
    static constexpr ImVec4 Border = { 0.725f, 0.725f, 0.725f, 1.0f };
    static constexpr ImVec4 Separator = { 0.784f, 0.784f, 0.784f, 1.0f };

    // Text
    static constexpr ImVec4 Text = { 0.118f, 0.118f, 0.118f, 1.0f };
    static constexpr ImVec4 TextDisabled = { 0.502f, 0.502f, 0.502f, 1.0f };

    // Scrollbar
    static constexpr ImVec4 ScrollbarBg = { 0.902f, 0.902f, 0.902f, 1.0f };
    static constexpr ImVec4 ScrollbarGrab = { 0.725f, 0.725f, 0.725f, 1.0f };
    static constexpr ImVec4 ScrollbarGrabHover = { 0.627f, 0.627f, 0.627f, 1.0f };
    static constexpr ImVec4 ScrollbarGrabActive = { 0.529f, 0.529f, 0.529f, 1.0f };

    // Misc
    static constexpr ImVec4 CheckMark = { 0.820f, 0.360f, 0.050f, 1.0f };
    static constexpr ImVec4 SliderGrab = { 0.627f, 0.627f, 0.627f, 1.0f };
    static constexpr ImVec4 SliderGrabActive = { 0.820f, 0.360f, 0.050f, 1.0f };
    static constexpr ImVec4 ResizeGrip = { 0.820f, 0.360f, 0.050f, 0.25f };
    static constexpr ImVec4 ResizeGripHovered = { 0.820f, 0.360f, 0.050f, 0.67f };
    static constexpr ImVec4 ResizeGripActive = { 0.820f, 0.360f, 0.050f, 0.95f };
};

class CImGuiThemes
{
public:
    static void ApplyDarkModeTheme();
    static void ApplyLightModeTheme();
};
