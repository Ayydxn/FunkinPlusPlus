#include "FunkinPCH.h"
#include "Logging.h"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

void CLogging::Initialize()
{
    if (bIsInitialized)
    {
        m_Logger->warn("Logging has already been initialized! It cannot be initialized more than once!");
        return;
    }
    
    const std::filesystem::path LogsDirectory = GetLogsDirectory();

    if (!std::filesystem::exists(LogsDirectory))
        std::filesystem::create_directory(LogsDirectory);
    
    const std::filesystem::path LogFileName = LogsDirectory / "FunkinPlusPlus-latest.log";

    // If the "latest" log file exists, we rename it. This allows for the backup of older log files.
    if (std::filesystem::exists(LogFileName))
    {
        const std::string BackupName = std::format("FunkinPlusPlus-backup-{}-{}.log", GetDateAsString(), GetBackupLogID());
        const std::filesystem::path BackupPath = LogsDirectory / BackupName;

        std::filesystem::rename(LogFileName, BackupPath);
    }

    std::vector<spdlog::sink_ptr> LoggerSinks =
    {
        std::make_shared<spdlog::sinks::basic_file_sink_mt>(LogFileName.string(), true),

        // The console is disabled in Distribution builds, so we only create a console logger if we aren't building in Distribution.
        #ifndef FUNKIN_BUILD_DISTRIBUTION
            std::make_shared<spdlog::sinks::stdout_color_sink_mt>()
        #endif
    };

    LoggerSinks[0]->set_pattern("[%I:%M:%S.%e] [%l] (%n): %v");

    #ifndef FUNKIN_BUILD_DISTRIBUTION
        LoggerSinks[1]->set_pattern("[%I:%M:%S.%e] %^[%l] (%n): %v%$");
    #endif

    m_Logger = std::make_shared<spdlog::logger>("Funkin++", LoggerSinks.begin(), LoggerSinks.end());
    
    #ifndef FUNKIN_BUILD_DISTRIBUTION
        m_Logger->set_level(spdlog::level::trace);
    #else
        m_Logger->set_level(spdlog::level::info);
    #endif

    bIsInitialized = true;
}

void CLogging::Shutdown()
{
    m_Logger.reset();

    spdlog::drop_all();

    bIsInitialized = false;
}

std::string CLogging::GetDateAsString()
{
    const std::chrono::zoned_time TimeNow { std::chrono::current_zone(), std::chrono::system_clock::now() };
    
    return std::format("{:%m.%d.%Y}", TimeNow);
}

std::string CLogging::GetBackupLogID()
{
    const auto Now = std::chrono::floor<std::chrono::milliseconds>(std::chrono::system_clock::now());
    const std::chrono::zoned_time TimeNow { std::chrono::current_zone(), Now };

    return std::format("{:%H-%M-%S}", TimeNow);
}

std::filesystem::path CLogging::GetLogsDirectory()
{
    return std::filesystem::path(PROJECT_ROOT) / "Logs";
}
