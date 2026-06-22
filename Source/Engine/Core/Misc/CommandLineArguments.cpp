#include "FunkinPCH.h"
#include "CommandLineArguments.h"

CCommandLineArguments::CCommandLineArguments(int32 ArgumentCount, char** Arguments)
{
    m_CommandLineArguments.reserve(ArgumentCount);

    for (int32 i = 0; i < ArgumentCount; i++)
    {
        // All command line arguments must be prefixed with a '-'.
        // So if the argument at the current index isn't prefixed with it, we ignore it.
        // This also gets rid of the path to the application's executable which is always the first argument regardless of what is passed. 
        if (Arguments[i][0] != '-')
            continue;

        m_CommandLineArguments.emplace_back(Arguments[i]);
    }
}