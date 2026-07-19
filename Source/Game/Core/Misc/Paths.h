#pragma once

#include <filesystem>

class CPaths
{
public:
    static void Initialize();

    static const std::filesystem::path& GetRootDirectory();
    static const std::filesystem::path& GetAssetsDirectory();
    static const std::filesystem::path& GetShadersDirectory();
    static const std::filesystem::path& GetShaderCacheDirectory();
private:
    static std::filesystem::path GetGameDirectory();
    
    static bool MakeDirectory(const std::filesystem::path& Directory);
private:
    inline static std::filesystem::path m_RootDirectory = "";
    inline static std::filesystem::path m_AssetsDirectory = "";
    inline static std::filesystem::path m_ShadersDirectory = "";
    inline static std::filesystem::path m_ShaderCacheDirectory = "";

    inline static bool bIsInitialized = false;
};
