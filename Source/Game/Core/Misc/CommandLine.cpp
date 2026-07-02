#include "FunkinPCH.h"
#include "CommandLine.h"
#include "CommandLineParser.h"

std::unique_ptr<CCommandLine> CCommandLine::m_Instance = nullptr;

CCommandLine::CCommandLine(int32 ArgumentsCount, char** Arguments)
    : m_CommandLineArguments(ArgumentsCount, Arguments) {}

void CCommandLine::Initialize(int32 ArgumentsCount, char** Arguments)
{
    if (m_Instance == nullptr)
        m_Instance = std::make_unique<CCommandLine>(ArgumentsCount, Arguments);
}

bool CCommandLine::Flag(const std::string& Argument) const
{
    return CCommandLineParser::Flag(m_CommandLineArguments, Argument);
}

bool CCommandLine::Value(const std::string& Argument, int32* Value) const
{
    return CCommandLineParser::Value(m_CommandLineArguments, Argument, Value);
}

bool CCommandLine::Value(const std::string& Argument, float* Value) const
{
    return CCommandLineParser::Value(m_CommandLineArguments, Argument, Value);
}

bool CCommandLine::Value(const std::string& Argument, double* Value) const
{
    return CCommandLineParser::Value(m_CommandLineArguments, Argument, Value);
}

bool CCommandLine::Value(const std::string& Argument, std::string* Value) const
{
    return CCommandLineParser::Value(m_CommandLineArguments, Argument, Value);
}
