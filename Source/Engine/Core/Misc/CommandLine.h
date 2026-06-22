#pragma once

#include "CommandLineParser.h"

#include <memory>

class FUNKIN_API CCommandLine
{
public:
    CCommandLine(const CCommandLine&) = delete;
    CCommandLine& operator=(const CCommandLine&) = delete;
    
    static void Initialize(int32 ArgumentsCount, char** Arguments);
    
    bool Flag(const std::string& Argument) const;
    
    static CCommandLine& Get() { return *m_Instance; }
public:
    CCommandLine(int32 ArgumentsCount, char** Arguments);
    ~CCommandLine() = default;
private:
    static std::unique_ptr<CCommandLine> m_Instance;
    
    CCommandLineArguments m_CommandLineArguments;
};
