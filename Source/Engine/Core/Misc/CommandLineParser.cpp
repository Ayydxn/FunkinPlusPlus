#include "FunkinPCH.h"
#include "CommandLineParser.h"

bool CCommandLineParser::Flag(const CCommandLineArguments& CommandLineArguments, const std::string& Argument)
{
    return std::ranges::find(CommandLineArguments, '-' + Argument) != CommandLineArguments.end();
}

bool CCommandLineParser::Value(const CCommandLineArguments& CommandLineArguments, const std::string& Argument, int32* Value)
{
    return std::ranges::any_of(CommandLineArguments, [Argument, Value](const std::string& CommandLineArgument) mutable
    {
        if (CommandLineArgument.find('-' + Argument) != std::string::npos)
        {
            const std::string ArgumentValueString = CommandLineArgument.substr(CommandLineArgument.find('=') + 1);

            *Value = std::stoi(ArgumentValueString);

            return Value != nullptr;
        }
        
        return false;
    });
}

bool CCommandLineParser::Value(const CCommandLineArguments& CommandLineArguments, const std::string& Argument, float* Value)
{
    return std::ranges::any_of(CommandLineArguments, [Argument, Value](const std::string& CommandLineArgument) mutable
    {
        if (CommandLineArgument.find('-' + Argument) != std::string::npos)
        {
            const std::string ArgumentValueString = CommandLineArgument.substr(CommandLineArgument.find('=') + 1);

            *Value = std::stof(ArgumentValueString);

            return Value != nullptr;
        }
        
        return false;
    });
}

bool CCommandLineParser::Value(const CCommandLineArguments& CommandLineArguments, const std::string& Argument, double* Value)
{
    return std::ranges::any_of(CommandLineArguments, [Argument, Value](const std::string& CommandLineArgument) mutable
    {
        if (CommandLineArgument.find('-' + Argument) != std::string::npos)
        {
            const std::string ArgumentValueString = CommandLineArgument.substr(CommandLineArgument.find('=') + 1);

            *Value = std::stod(ArgumentValueString);

            return Value != nullptr;
        }
        
        return false;
    });
}

bool CCommandLineParser::Value(const CCommandLineArguments& CommandLineArguments, const std::string& Argument, std::string* Value)
{
    return std::ranges::any_of(CommandLineArguments, [Argument, Value](const std::string& CommandLineArgument) mutable
    {
        if (CommandLineArgument.find('-' + Argument) != std::string::npos)
        {
            *Value = CommandLineArgument.substr(CommandLineArgument.find('=') + 1);

            return !Value->empty();
        }
        
        return false;
    });
}
