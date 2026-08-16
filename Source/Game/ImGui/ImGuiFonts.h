#pragma once

struct FFontConfiguration
{
    std::string Name;
    std::string Filepath;
    float Size;
};

class CImGuiFonts
{
public:
    static void Add(const FFontConfiguration& Configuration, bool bIsDefault = false);
};
