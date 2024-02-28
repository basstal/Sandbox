#include "Logger.hpp"


std::ofstream Logger::m_fileStream = {};

std::ostream* Logger::m_outputStream = &std::cout;

bool Logger::m_useColor = true;

std::vector<std::string> Logger::m_messages = {};

void Logger::CloseLogFile()
{
    if (m_fileStream.is_open())
    {
        m_fileStream.close();
    }
}

void Logger::RedirectOutputToFile(const std::string& filePath)
{
    CloseLogFile();
    m_fileStream.open(filePath, std::ofstream::out | std::ofstream::app);
    if (!m_fileStream)
    {
        std::cerr << "Failed to open log file: " << filePath << '\n';
        return;
    }
    m_outputStream = &m_fileStream;
    m_useColor = false; // 文件输出不使用颜色
}

void Logger::SetOutputToConsole()
{
    CloseLogFile();
    m_outputStream = &std::cout;
    m_useColor = true; // 控制台输出使用颜色
}
