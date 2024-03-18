#pragma once
#include <format>
#include <stdexcept>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>

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
        static void Debug(const std::string& format, Args&&... args);

        template <typename... Args>
        static void Info(const std::string& format, Args&&... args);

        template <typename... Args>
        static void Warning(const std::string& format, Args&&... args);

        template <typename... Args>
        static void Error(const std::string& format, Args&&... args);

        template <typename... Args>
        static void Fatal(const std::string& format, Args&&... args);

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
        /**
         * \brief 日志消息
         */
        static std::vector<std::string> m_messages;
    };


    template <typename... Args>
    void Logger::Log(LoggerLevel level, const std::string& format, Args&&... args)
    {
        auto message = std::vformat(format, std::make_format_args(std::forward<Args>(args)...));
        const char* prefix = "";
        const char* colorCode = "";
        switch (level)
        {
        case LevelFatal:
            prefix = "[FATAL]: ";
            colorCode = "\x1b[31m"; // 红色
            break;
        case LevelError:
            prefix = "[ERROR]: ";
            colorCode = "\x1b[31m"; // 红色
            break;
        case LevelWarning:
            prefix = "[WARNING]: ";
            colorCode = "\x1b[33m"; // 黄色
            break;
        case LevelInfo:
            prefix = "[INFO]: ";
            colorCode = "\x1b[32m"; // 绿色
            break;
        case LevelDebug:
            prefix = "[DEBUG]: ";
            colorCode = "";
            break;
        }

        if (m_useColor)
        {
            *m_outputStream << colorCode << prefix << message << "\x1b[0m" << std::endl;
        }
        else
        {
            *m_outputStream << prefix << message << std::endl;
        }

        // 存储日志消息以便稍后写入文件
        m_messages.push_back(std::string(prefix) + message);

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
}

#define LOGI(...) Sandbox::Logger::Info(__VA_ARGS__);
#define LOGW(...) Sandbox::Logger::Warning(__VA_ARGS__);
#define LOGE(...) Sandbox::Logger::Error("[{}:{}] {}", __FILE__, __LINE__, std::format(__VA_ARGS__));
#define LOGF(...) Sandbox::Logger::Fatal("[{}:{}] {}", __FILE__, __LINE__, std::format(__VA_ARGS__));
#define LOGD(...) Sandbox::Logger::Debug(__VA_ARGS__);
