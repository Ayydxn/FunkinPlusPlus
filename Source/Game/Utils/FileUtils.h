#pragma once

class CFileUtils
{
public:
    static std::string ReadFile(const std::filesystem::path& Filepath);
    static std::vector<uint8_t> ReadBinaryFile(const std::filesystem::path& FilePath);
    static std::string RedactUserFolderFromFilepath(const std::filesystem::path& Filepath);
};
