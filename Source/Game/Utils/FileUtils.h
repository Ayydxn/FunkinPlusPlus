#pragma once

class CFileUtils
{
public:
    static std::string CFileUtils::ReadFile(const std::filesystem::path& Filepath);
    static std::string RedactUserFolderFromFilepath(const std::filesystem::path& Filepath);
};
