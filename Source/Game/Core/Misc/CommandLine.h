#pragma once

#include "CommandLineParser.h"

#include <memory>

class CCommandLine
{
public:
    CCommandLine(const CCommandLine&) = delete;
    CCommandLine& operator=(const CCommandLine&) = delete;
    
    static void Initialize(int32 ArgumentsCount, char** Arguments);
    
    bool Flag(const std::string& Argument) const;
    
    bool Value(const std::string& Argument, int32* Value) const;
    bool Value(const std::string& Argument, float* Value) const;
    bool Value(const std::string& Argument, double* Value) const;
    bool Value(const std::string& Argument, std::string* Value) const;
    
    static CCommandLine& Get() { return *m_Instance; }
public:
    CCommandLine(int32 ArgumentsCount, char** Arguments);
    ~CCommandLine() = default;
private:
    static std::unique_ptr<CCommandLine> m_Instance;
    
    CCommandLineArguments m_CommandLineArguments;
};
