#pragma once
#include <format>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "Misc/Event.hpp"

namespace Sandbox
{
    class Logger
    {
    public:
        enum LoggerLevel
        {
            /**
             * \brief 致命错误
             */
            LevelFatal,
            /**
             * \brief 错误
             */
            LevelError,
            /**
             * \brief 警告
             */
            LevelWarning,
            /**
             * \brief 信息
             */
            LevelInfo,
            /**
             * \brief 调试
             */
            LevelDebug,
        };

        struct LogMessage
        {
            LoggerLevel level;
            std::string tag;
            std::string message;
        };

        /**
         * \brief 重定向输出到文件
         */
        static void RedirectOutputToFile(const std::string& filePath);

        /**
         * \brief 输出到控制台
         */
        static void SetOutputToConsole();

        /**
         * \brief 输出日志
         * \tparam Args
         * \param level 日志级别
         * \param format 格式化字符串
         * \param args 参数
         */
        template <typename... Args>
        static void Log(LoggerLevel level, const std::string& format, Args&&... args);

        template <typename... Args>
        static void Log(LoggerLevel level, const std::string& tag, const std::string& format, Args&&... args);


        template <typename... Args>
        static void Debug(const std::string& format, Args&&... args);

        template <typename... Args>
        static void Info(const std::string& format, Args&&... args);

        template <typename... Args>
        static void Warning(const std::string& format, Args&&... args);

        template <typename... Args>
        static void Error(const std::string& format, Args&&... args);

        template <typename... Args>
        static void Fatal(const std::string& format, Args&&... args);


        template <typename... Args>
        static void DebugTag(const std::string& tag, const std::string& format, Args&&... args);

        template <typename... Args>
        static void InfoTag(const std::string& tag, const std::string& format, Args&&... args);

        template <typename... Args>
        static void WarningTag(const std::string& tag, const std::string& format, Args&&... args);

        template <typename... Args>
        static void ErrorTag(const std::string& tag, const std::string& format, Args&&... args);

        template <typename... Args>
        static void FatalTag(const std::string& tag, const std::string& format, Args&&... args);

        static std::vector<LogMessage> GetMessagesByTag(const std::string& tag);
        /**
         * \brief 日志消息
         */
        static std::vector<LogMessage> messages;

        static std::map<LoggerLevel, std::tuple<std::string, std::string>> levelDetails;

        static Event<const LogMessage&> onLogMessage;

    private:
        /**
         * \brief 关闭日志文件
         */
        static void CloseLogFile();

        /**
         * \brief 文件输出流
         */
        static std::ofstream m_fileStream;
        /**
         * \brief 输出流
         */
        static std::ostream* m_outputStream;
        /**
         * \brief 是否使用颜色
         */
        static bool m_useColor;
    };


    template <typename... Args>
    void Logger::Log(LoggerLevel level, const std::string& format, Args&&... args)
    {
        Log(level, "", format, std::forward<Args>(args)...);
    }
    template <typename... Args>
    void Logger::Log(LoggerLevel level, const std::string& tag, const std::string& format, Args&&... args)
    {
        auto message                  = std::vformat(format, std::make_format_args(std::forward<Args>(args)...));
        auto [levelPrefix, colorCode] = levelDetails[level];
        auto formattedTag             = tag.empty() ? tag : std::vformat("[{}] ", std::make_format_args(tag));
        if (m_useColor)
        {
            *m_outputStream << colorCode << levelPrefix << formattedTag << message << "\x1b[0m" << std::endl;
        }
        else
        {
            *m_outputStream << levelPrefix << formattedTag << message << std::endl;
        }

        // 将消息添加到 m_messages，现在包含 tag
        LogMessage logMessage = {level, formattedTag, message};
        messages.push_back(logMessage);
        onLogMessage.Trigger(logMessage);
        if (level == LevelFatal)
        {
            throw std::runtime_error(message);
        }
    }

    template <typename... Args>
    void Logger::Debug(const std::string& format, Args&&... args)
    {
        Log(LevelDebug, format, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void Logger::Info(const std::string& format, Args&&... args)
    {
        Log(LevelInfo, format, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void Logger::Warning(const std::string& format, Args&&... args)
    {
        Log(LevelWarning, format, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void Logger::Error(const std::string& format, Args&&... args)
    {
        Log(LevelError, format, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void Logger::Fatal(const std::string& format, Args&&... args)
    {
        Log(LevelFatal, format, std::forward<Args>(args)...);
    }
    template <typename... Args>
    void Logger::DebugTag(const std::string& tag, const std::string& format, Args&&... args)
    {
        Log(LevelDebug, tag, format, std::forward<Args>(args)...);
    }
    template <typename... Args>
    void Logger::InfoTag(const std::string& tag, const std::string& format, Args&&... args)
    {
        Log(LevelInfo, tag, format, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void Logger::WarningTag(const std::string& tag, const std::string& format, Args&&... args)
    {
        Log(LevelWarning, tag, format, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void Logger::ErrorTag(const std::string& tag, const std::string& format, Args&&... args)
    {
        Log(LevelError, tag, format, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void Logger::FatalTag(const std::string& tag, const std::string& format, Args&&... args)
    {
        Log(LevelFatal, tag, format, std::forward<Args>(args)...);
    }


}  // namespace Sandbox

#define LOGI_OLD(...) Sandbox::Logger::Info(__VA_ARGS__);
#define LOGW_OLD(...) Sandbox::Logger::Warning(__VA_ARGS__);
#define LOGE_OLD(...) Sandbox::Logger::Error("[{}:{}] {}", __FILE__, __LINE__, std::format(__VA_ARGS__));
#define LOGF_OLD(...) Sandbox::Logger::Fatal("[{}:{}] {}", __FILE__, __LINE__, std::format(__VA_ARGS__));
#define LOGD_OLD(...) Sandbox::Logger::Debug(__VA_ARGS__);

#define LOGI(tag, ...) Sandbox::Logger::InfoTag(tag, __VA_ARGS__);
#define LOGW(tag, ...) Sandbox::Logger::WarningTag(tag, __VA_ARGS__);
#define LOGE(tag, ...) Sandbox::Logger::ErrorTag(tag, "[{}:{}] {}", __FILE__, __LINE__, std::format(__VA_ARGS__));
#define LOGF(tag, ...) Sandbox::Logger::FatalTag(tag, "[{}:{}] {}", __FILE__, __LINE__, std::format(__VA_ARGS__));
#define LOGD(tag, ...) Sandbox::Logger::DebugTag(tag, __VA_ARGS__);