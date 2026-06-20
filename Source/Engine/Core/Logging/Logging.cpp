#include "FunkinPCH.h"
#include "Logging.h"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

void CLogging::Initialize()
{
    if (bIsInitialized)
    {
        m_EngineLogger->warn("Logging has already been initialized! It cannot be initialized more than once!");
        return;
    }

    if (!std::filesystem::exists("Logs"))
        std::filesystem::create_directory("Logs");
    
    std::string LogFileName = "Logs/FunkinPlusPlus-latest.log";

    // If the "latest" log file exists, we rename it. This allows for the backup of older log files.
    // The same thing is done with the application's log file.
    if (std::filesystem::exists(LogFileName))
    {
        std::stringstream StringStream;
        StringStream << std::format("Logs/FunkinPlusPlus-backup-{}-{}.log", GetDateAsString(), GetBackupLogID());

        std::filesystem::rename(LogFileName, StringStream.str());
    }

    std::vector<spdlog::sink_ptr> LoggerSinks =
    {
        std::make_shared<spdlog::sinks::basic_file_sink_mt>(LogFileName, true),

        // The console is disabled in Distribution builds, so we only create a console logger if we aren't building in Distribution.
        #ifndef FUNKIN_BUILD_DISTRIBUTION
            std::make_shared<spdlog::sinks::stdout_color_sink_mt>()
        #endif
    };

    LoggerSinks[0]->set_pattern("[%I:%M:%S.%e] [%l] (%n): %v");

    #ifndef FUNKIN_BUILD_DISTRIBUTION
        LoggerSinks[1]->set_pattern("[%I:%M:%S.%e] %^[%l] (%n): %v%$");
    #endif

    m_EngineLogger = std::make_shared<spdlog::logger>("Funkin Engine", LoggerSinks.begin(), LoggerSinks.end());
    m_EngineLogger->set_level(spdlog::level::trace);
    
    m_GameLogger = std::make_shared<spdlog::logger>("FunkinPlusPlus", LoggerSinks.begin(), LoggerSinks.end());
    m_GameLogger->set_level(spdlog::level::trace);

    bIsInitialized = true;
}

void CLogging::Shutdown()
{
    m_EngineLogger.reset();
    m_GameLogger.reset();

    spdlog::drop_all();

    bIsInitialized = false;
}

std::string CLogging::GetDateAsString()
{
    const auto TimeNow = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::tm Time = {};
    
    #ifdef FUNKIN_PLATFORM_WIN64
        assertEngine(localtime_s(&Time, &TimeNow) == 0)
    #else
        assertEngine(localtime_r(&TimeNow, &Time) == 0)
    #endif

    std::string DayString = std::to_string(Time.tm_mday);
    std::string MonthString = std::to_string(Time.tm_mon + 1);
    std::string YearString = std::to_string(Time.tm_year + 1900);

    return std::format("{}.{}.{}", MonthString, DayString, YearString);
}

std::string CLogging::GetBackupLogID()
{
    const auto Seed = static_cast<uint32>(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    std::default_random_engine RandomEngine(Seed);
    std::uniform_int_distribution Distribution(10, 100);

    std::stringstream StringStream;
    StringStream << std::format("{}.{}.{}", Distribution(RandomEngine), Distribution(RandomEngine), Distribution(RandomEngine));

    return StringStream.str();
}
