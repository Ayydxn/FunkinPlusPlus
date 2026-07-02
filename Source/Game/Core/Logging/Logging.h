#pragma once

#undef FMT_STRING
#undef FMT_STRING_IMPL
#define FMT_UNICODE 0

#include "Core/CoreTypes.h"

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

#include <memory>

enum class ELogLevel : uint8
{
    Trace,
    Info,
    Debug,
    Warning,
    Error,
    Critical
};

class CLogging
{
public:
    static void Initialize();
    static void Shutdown();

    template<typename... Arguments>
    static void LogMessage(ELogLevel LogLevel, std::string_view Tag, std::string_view Message, Arguments&& ... Args);

    template<typename... Arguments>
    static void LogAssertMessage(std::string_view Prefix, std::string_view Message, Arguments&& ... Args);
private:
    static std::string GetDateAsString();
    static std::string GetBackupLogID();
    static std::filesystem::path GetLogsDirectory();
private:
    inline static std::shared_ptr<spdlog::logger> m_Logger;

    inline static bool bIsInitialized = false;
};

template<typename... Arguments>
void CLogging::LogMessage(ELogLevel LogLevel, std::string_view Tag, std::string_view Message, Arguments&& ... Args)
{
    const std::string LogString = Tag.empty() ? "{}{}" : "[{}] {}";

    switch (LogLevel)
    {
        case ELogLevel::Trace:
        {
            // (Ayydan) Spdlog uses the fmt library to format log messages.
            // Since we build with C++20, fmt by default has compile-time checks for format strings enabled.
            // So we do this to bypass those compile-time checks.
            m_Logger->trace(fmt::runtime(LogString), Tag, fmt::vformat(Message, fmt::make_format_args(Args...)));
            break;
        }

        case ELogLevel::Info:
        {
            m_Logger->info(fmt::runtime(LogString), Tag, fmt::vformat(Message, fmt::make_format_args(Args...)));
            break;
        }

        case ELogLevel::Debug:
        {
            m_Logger->debug(fmt::runtime(LogString), Tag, fmt::vformat(Message, fmt::make_format_args(Args...)));
            break;
        }

        case ELogLevel::Warning:
        {
            m_Logger->warn(fmt::runtime(LogString), Tag, fmt::vformat(Message, fmt::make_format_args(Args...)));
            break;
        }

        case ELogLevel::Error:
        {
            m_Logger->error(fmt::runtime(LogString), Tag, fmt::vformat(Message, fmt::make_format_args(Args...)));
            break;
        }

        case ELogLevel::Critical:
        {
            m_Logger->critical(fmt::runtime(LogString), Tag, fmt::vformat(Message, fmt::make_format_args(Args...)));
            break;
        }
    }
}

template <typename... Arguments>
void CLogging::LogAssertMessage(std::string_view Prefix, std::string_view Message, Arguments&&... Args)
{
    m_Logger->error("{}: {}", Prefix, fmt::vformat(Message, fmt::make_format_args(Args...)));
}

/*----------------------*/
/* -- Logging Macros -- */
/*----------------------*/

#define LOG_TRACE_TAG(Tag, Message, ...)         CLogging::LogMessage(ELogLevel::Trace, Tag, Message, ##__VA_ARGS__)
#define LOG_INFO_TAG(Tag, Message, ...)          CLogging::LogMessage(ELogLevel::Info, Tag, Message, ##__VA_ARGS__)
#define LOG_DEBUG_TAG(Tag, Message, ...)         CLogging::LogMessage(ELogLevel::Debug, Tag, Message, ##__VA_ARGS__)
#define LOG_WARN_TAG(Tag, Message, ...)          CLogging::LogMessage(ELogLevel::Warning, Tag, Message, ##__VA_ARGS__)
#define LOG_ERROR_TAG(Tag, Message, ...)         CLogging::LogMessage(ELogLevel::Error, Tag, Message, ##__VA_ARGS__)
#define LOG_CRITICAL_TAG(Tag, Message, ...)      CLogging::LogMessage(ELogLevel::Critical, Tag, Message, ##__VA_ARGS__)

#define LOG_TRACE(Message, ...)                  CLogging::LogMessage(ELogLevel::Trace, "", Message, ##__VA_ARGS__)
#define LOG_INFO(Message, ...)                   CLogging::LogMessage(ELogLevel::Info,  "", Message, ##__VA_ARGS__)
#define LOG_DEBUG(Message, ...)                  CLogging::LogMessage(ELogLevel::Debug, "", Message, ##__VA_ARGS__)
#define LOG_WARN(Message, ...)                   CLogging::LogMessage(ELogLevel::Warning, "", Message, ##__VA_ARGS__)
#define LOG_ERROR(Message, ...)                  CLogging::LogMessage(ELogLevel::Error, "", Message, ##__VA_ARGS__)
#define LOG_CRITICAL(Message, ...)               CLogging::LogMessage(ELogLevel::Critical, "", Message, ##__VA_ARGS__)
