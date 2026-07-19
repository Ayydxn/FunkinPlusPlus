#include "FunkinPCH.h"
#include "Paths.h"
#include "Utils/FileUtils.h"

#include <SDL3/SDL.h>

void CPaths::Initialize()
{
    if (bIsInitialized)
        return;

    const char* BasePath = SDL_GetBasePath();
    verifyFunkinf(BasePath, "Failed to query the executable's base path via SDL! ({})", SDL_GetError())

    const std::filesystem::path ExecutableDirectory = std::filesystem::path(BasePath).lexically_normal();
    
    m_RootDirectory = ExecutableDirectory.parent_path().parent_path().parent_path();
    m_AssetsDirectory = m_RootDirectory / "Assets";
    m_ShadersDirectory = m_AssetsDirectory / "Shaders";
    m_ShaderCacheDirectory = GetGameDirectory() / "ShaderCache";
    
    verifyFunkinf(MakeDirectory(m_ShaderCacheDirectory), "Failed to create shader cache directory!")
    
    bIsInitialized = true;

    LOG_INFO_TAG("Core", "Resolved game root directory: '{}'", CFileUtils::RedactUserFolderFromFilepath(m_RootDirectory));
}

const std::filesystem::path& CPaths::GetRootDirectory()
{
    verifyFunkin(bIsInitialized)
    
    return m_RootDirectory;
}
 
const std::filesystem::path& CPaths::GetAssetsDirectory()
{
    verifyFunkin(bIsInitialized)
    
    return m_AssetsDirectory;
}
 
const std::filesystem::path& CPaths::GetShadersDirectory()
{
    verifyFunkin(bIsInitialized)
    
    return m_ShadersDirectory;
}
 
const std::filesystem::path& CPaths::GetShaderCacheDirectory()
{
    verifyFunkin(bIsInitialized)
    
    return m_ShaderCacheDirectory;
}

std::filesystem::path CPaths::GetGameDirectory()
{
    char* RawRootGameDirectory = SDL_GetPrefPath("", "Friday Night Funkin'++");
    verifyFunkinf(RawRootGameDirectory, "Failed to get the root of the game directory via SDL! ({})", SDL_GetError())

    const std::filesystem::path RootGameDirectory(RawRootGameDirectory);
    SDL_free(RawRootGameDirectory);
    
    #ifdef FUNKIN_PLATFORM_WIN64
        const auto GameDirectoryName = RootGameDirectory.parent_path().filename();
        const auto AppData = RootGameDirectory.parent_path().parent_path().parent_path();
        
        std::filesystem::path GameDirectory = AppData / "Local" / GameDirectoryName;
        
        if (!std::filesystem::exists(GameDirectory))
            verifyFunkin(std::filesystem::create_directories(GameDirectory))
        
        return GameDirectory;
    #else
        return RootGameDirectory; 
    #endif
}

bool CPaths::MakeDirectory(const std::filesystem::path& Directory)
{
    if (!std::filesystem::exists(Directory))
        return std::filesystem::create_directories(Directory);
    
    return true;
}
