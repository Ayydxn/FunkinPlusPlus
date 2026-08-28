#include "FunkinPCH.h"
#include "FileUtils.h"

std::string CFileUtils::ReadFile(const std::filesystem::path& Filepath)
{
    std::ifstream FileReader(Filepath, std::ios::in | std::ios::binary);

    if (!FileReader.is_open())
    {
        LOG_ERROR_TAG("IO", "Failed to open file '{}' for reading!", RedactUserFolderFromFilepath(Filepath));
        return {};
    }

    std::string Contents;

    FileReader.seekg(0, std::ios::end);
    Contents.resize(FileReader.tellg());
    FileReader.seekg(0, std::ios::beg);

    FileReader.read(Contents.data(), static_cast<int64>(Contents.size()));

    return Contents;
}

std::vector<uint8_t> CFileUtils::ReadBinaryFile(const std::filesystem::path& FilePath)
{
    std::ifstream File(FilePath, std::ios::binary | std::ios::ate);
    if (!File.is_open())
        return {};

    const auto FileSize = File.tellg();
    if (FileSize <= 0)
        return {};

    std::vector<uint8_t> Buffer(FileSize);
    File.seekg(0, std::ios::beg);
    File.read(reinterpret_cast<char*>(Buffer.data()), FileSize);
    
    return Buffer;
}

std::string CFileUtils::RedactUserFolderFromFilepath(const std::filesystem::path& Filepath)
{
    // Evaluated safely exactly once on the first function call
    static const std::filesystem::path HomePath = []()
    {
        #ifdef FUNKIN_PLATFORM_WIN64
            const char* UserFolderEnvVarName = "USERPROFILE";
        #else
            const char* UserFolderEnvVarName = "HOME";
        #endif
        
        const char* EnvVarValue = std::getenv(UserFolderEnvVarName);
        return EnvVarValue ? std::filesystem::path(EnvVarValue) : std::filesystem::path();
    }();

    // Early exit if the home directory could not be resolved
    if (HomePath.empty())
        return Filepath.string();

    // Use filesystem logic instead of string matching to handle path variations safely
    if (Filepath.string().starts_with(HomePath.string()))
    {
        const auto Relative = std::filesystem::relative(Filepath, HomePath);
        
        return (std::filesystem::path("~") / Relative).generic_string();
    }

    return Filepath.string();
}
