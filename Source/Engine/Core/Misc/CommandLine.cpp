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
