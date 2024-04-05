#include "pch.hpp"

#include "Logger.hpp"


std::ofstream Sandbox::Logger::m_fileStream = {};

std::ostream* Sandbox::Logger::m_outputStream = &std::cout;

bool Sandbox::Logger::m_useColor = true;

std::vector<Sandbox::Logger::LogMessage> Sandbox::Logger::messages = {};

std::map<Sandbox::Logger::LoggerLevel, std::tuple<std::string, std::string>> Sandbox::Logger::levelDetails = {
    {LevelFatal, {"[FATAL] ", "\x1b[31m"}},  // 红色
    {LevelError, {"[ERROR] ", "\x1b[31m"}},  // 红色
    {LevelWarning, {"[WARNING] ", "\x1b[33m"}},  // 黄色
    {LevelInfo, {"[INFO] ", "\x1b[32m"}},  // 绿色
    {LevelDebug, {"[DEBUG] ", ""}},
};

Sandbox::Event<const Sandbox::Logger::LogMessage&> Sandbox::Logger::onLogMessage = {};

std::vector<Sandbox::Logger::LogMessage> Sandbox::Logger::GetMessagesByTag(const std::string& tag)
{
    std::vector<LogMessage> filteredMessages;
    auto                    formattedTag = tag.empty() ? tag : std::vformat("[{}] ", std::make_format_args(tag));
    for (const auto& message : messages)
    {
        if (message.tag == formattedTag)
        {
            filteredMessages.push_back(message);
        }
    }

    return filteredMessages;
}

void Sandbox::Logger::CloseLogFile()
{
    if (m_fileStream.is_open())
    {
        m_fileStream.close();
    }
}

void Sandbox::Logger::RedirectOutputToFile(const std::string& filePath)
{
    CloseLogFile();
    m_fileStream.open(filePath, std::ofstream::out | std::ofstream::app);
    if (!m_fileStream)
    {
        std::cerr << "Failed to open log file: " << filePath << '\n';
        return;
    }
    m_outputStream = &m_fileStream;
    m_useColor     = false;  // 文件输出不使用颜色
}

void Sandbox::Logger::SetOutputToConsole()
{
    CloseLogFile();
    m_outputStream = &std::cout;
    m_useColor     = true; // 控制台输出使用颜色
}
