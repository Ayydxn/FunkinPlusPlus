#pragma once

#undef FMT_STRING
#undef FMT_STRING_IMPL
#define FMT_UNICODE 0

#include "Core/CoreDefines.h"
#include "Core/CoreTypes.h"

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

#include <memory>

enum class ELogType : uint8
{
    Engine,
    Game
};

enum class ELogLevel : uint8
{
    Trace,
    Info,
    Debug,
    Warning,
    Error,
    Critical
};

class FUNKIN_API CLogging
{
public:
    static void Initialize();
    static void Shutdown();

    template<typename... Arguments>
    static void LogMessage(ELogType LogType, ELogLevel LogLevel, std::string_view Tag, std::string_view Message, Arguments&& ... Args);

    template<typename... Arguments>
    static void LogAssertMessage(ELogType LogType, std::string_view Prefix, std::string_view Message, Arguments&& ... Args);
private:
    static std::string GetDateAsString();
    static std::string GetBackupLogID();
private:
    inline static std::shared_ptr<spdlog::logger> m_EngineLogger;
    inline static std::shared_ptr<spdlog::logger> m_GameLogger;

    inline static bool bIsInitialized = false;
};

template<typename... Arguments>
void CLogging::LogMessage(ELogType LogType, ELogLevel LogLevel, std::string_view Tag, std::string_view Message, Arguments&& ... Args)
{
    const auto Logger = LogType == ELogType::Engine ? m_EngineLogger : m_GameLogger;
    const std::string LogString = Tag.empty() ? "{}{}" : "[{}] {}";

    switch (LogLevel)
    {
        case ELogLevel::Trace:
        {
            // (Ayydan) Spdlog uses the fmt library to format log messages.
            // Since we build with C++20, fmt by default has compile-time checks for format strings enabled.
            // So we do this to bypass those compile-time checks.
            Logger->trace(fmt::runtime(LogString), Tag, fmt::vformat(Message, fmt::make_format_args(Args...)));
            break;
        }

        case ELogLevel::Info:
        {
            Logger->info(fmt::runtime(LogString), Tag, fmt::vformat(Message, fmt::make_format_args(Args...)));
            break;
        }

        case ELogLevel::Debug:
        {
            Logger->debug(fmt::runtime(LogString), Tag, fmt::vformat(Message, fmt::make_format_args(Args...)));
            break;
        }

        case ELogLevel::Warning:
        {
            Logger->warn(fmt::runtime(LogString), Tag, fmt::vformat(Message, fmt::make_format_args(Args...)));
            break;
        }

        case ELogLevel::Error:
        {
            Logger->error(fmt::runtime(LogString), Tag, fmt::vformat(Message, fmt::make_format_args(Args...)));
            break;
        }

        case ELogLevel::Critical:
        {
            Logger->critical(fmt::runtime(LogString), Tag, fmt::vformat(Message, fmt::make_format_args(Args...)));
            break;
        }
    }
}

template <typename... Arguments>
void CLogging::LogAssertMessage(ELogType LogType, std::string_view Prefix, std::string_view Message, Arguments&&... Args)
{
    const auto Logger = LogType == ELogType::Engine ? m_EngineLogger : m_GameLogger;
    Logger->error("{}: {}", Prefix, fmt::vformat(Message, fmt::make_format_args(Args...)));
}

/*-----------------------------*/
/* -- Engine Logging Macros -- */
/*-----------------------------*/

#define ENGINE_LOG_TRACE_TAG(Tag, Message, ...)         CLogging::LogMessage(ELogType::Engine, ELogLevel::Trace, Tag, Message, ##__VA_ARGS__)
#define ENGINE_LOG_INFO_TAG(Tag, Message, ...)          CLogging::LogMessage(ELogType::Engine, ELogLevel::Info, Tag, Message, ##__VA_ARGS__)
#define ENGINE_LOG_DEBUG_TAG(Tag, Message, ...)         CLogging::LogMessage(ELogType::Engine, ELogLevel::Debug, Tag, Message, ##__VA_ARGS__)
#define ENGINE_LOG_WARN_TAG(Tag, Message, ...)          CLogging::LogMessage(ELogType::Engine, ELogLevel::Warning, Tag, Message, ##__VA_ARGS__)
#define ENGINE_LOG_ERROR_TAG(Tag, Message, ...)         CLogging::LogMessage(ELogType::Engine, ELogLevel::Error, Tag, Message, ##__VA_ARGS__)
#define ENGINE_LOG_CRITICAL_TAG(Tag, Message, ...)      CLogging::LogMessage(ELogType::Engine, ELogLevel::Critical, Tag, Message, ##__VA_ARGS__)

#define ENGINE_LOG_TRACE(Message, ...)                  CLogging::LogMessage(ELogType::Engine, ELogLevel::Trace, "", Message, ##__VA_ARGS__)
#define ENGINE_LOG_INFO(Message, ...)                   CLogging::LogMessage(ELogType::Engine, ELogLevel::Info,  "", Message, ##__VA_ARGS__)
#define ENGINE_LOG_DEBUG(Message, ...)                  CLogging::LogMessage(ELogType::Engine, ELogLevel::Debug, "", Message, ##__VA_ARGS__)
#define ENGINE_LOG_WARN(Message, ...)                   CLogging::LogMessage(ELogType::Engine, ELogLevel::Warning, "", Message, ##__VA_ARGS__)
#define ENGINE_LOG_ERROR(Message, ...)                  CLogging::LogMessage(ELogType::Engine, ELogLevel::Error, "", Message, ##__VA_ARGS__)
#define ENGINE_LOG_CRITICAL(Message, ...)               CLogging::LogMessage(ELogType::Engine, ELogLevel::Critical, "", Message, ##__VA_ARGS__)

/*---------------------------*/              
/* -- Game Logging Macros -- */              
/*---------------------------*/              

#define GAME_LOG_TRACE_TAG(Tag, Message, ...)           CLogging::LogMessage(ELogType::Game, ELogLevel::Trace, Tag, Message, ##__VA_ARGS__)
#define GAME_LOG_INFO_TAG(Tag, Message, ...)            CLogging::LogMessage(ELogType::Game, ELogLevel::Info, Tag, Message, ##__VA_ARGS__)
#define GAME_LOG_DEBUG_TAG(Tag, Message, ...)           CLogging::LogMessage(ELogType::Game, ELogLevel::Debug, Tag, Message, ##__VA_ARGS__)
#define GAME_LOG_WARN_TAG(Tag, Message, ...)            CLogging::LogMessage(ELogType::Game, ELogLevel::Warning, Tag, Message, ##__VA_ARGS__)
#define GAME_LOG_ERROR_TAG(Tag, Message, ...)           CLogging::LogMessage(ELogType::Game, ELogLevel::Error, Tag, Message, ##__VA_ARGS__)
#define GAME_LOG_CRITICAL_TAG(Tag, Message, ...)        CLogging::LogMessage(ELogType::Game, ELogLevel::Critical, Tag, Message, ##__VA_ARGS__)

#define GAME_LOG_TRACE(Message, ...)                    CLogging::LogMessage(ELogType::Game, ELogLevel::Trace, "", Message, ##__VA_ARGS__)
#define GAME_LOG_INFO(Message, ...)                     CLogging::LogMessage(ELogType::Game, ELogLevel::Info,  "", Message, ##__VA_ARGS__)
#define GAME_LOG_DEBUG(Message, ...)                    CLogging::LogMessage(ELogType::Game, ELogLevel::Debug, "", Message, ##__VA_ARGS__)
#define GAME_LOG_WARN(Message, ...)                     CLogging::LogMessage(ELogType::Game, ELogLevel::Warning, "", Message, ##__VA_ARGS__)
#define GAME_LOG_ERROR(Message, ...)                    CLogging::LogMessage(ELogType::Game, ELogLevel::Error, "", Message, ##__VA_ARGS__)
#define GAME_LOG_CRITICAL(Message, ...)                 CLogging::LogMessage(ELogType::Game, ELogLevel::Critical, "", Message, ##__VA_ARGS__)