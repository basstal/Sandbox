#include "pch.hpp"

#include "String.hpp"

#include "Generated/String.rfks.h"

Sandbox::String::String(const std::string& inString)
{
    m_string = inString;
    Sync();
}

Sandbox::String::String(const char* string)
{
    m_string = string;
    Sync();
}

Sandbox::String::String(const Sandbox::String& other)
{
    m_string = other.m_string;
    Sync();
}

Sandbox::String::String(Sandbox::String&& other) noexcept
{
    m_string = std::move(other.m_string);
    Sync();
}

Sandbox::String& Sandbox::String::operator=(const char* inString)
{
    m_string = inString;
    Sync();
    return *this;
}
Sandbox::String& Sandbox::String::operator=(const std::string& inString)
{
    m_string = inString;
    Sync();
    return *this;
}

Sandbox::String& Sandbox::String::operator=(const String& inString)
{
    if (&inString == this)
    {
        return *this;
    }
    m_string = inString.m_string;
    Sync();
    return *this;
}

void Sandbox::String::Construct(const std::string& inString)
{
    m_string = inString;
    Sync();
}

std::vector<std::string> Sandbox::String::Split(const std::string& source, const char& seperator)
{
    std::vector<std::string> reuslt;
    std::istringstream       istringstream(source);
    std::string              token;

    while (std::getline(istringstream, token, seperator))
    {
        reuslt.emplace_back(token);
    }
    return reuslt;
}

std::string Sandbox::String::Replace(const std::string& source, const std::string& from, const std::string& to)
{
    std::string result    = source;
    size_t      start_pos = 0;
    while ((start_pos = result.find(from, start_pos)) != std::string::npos)
    {
        result.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
    return result;
}

std::string Sandbox::String::ToStdString() const { return m_string; }

void Sandbox::String::Sync()
{
    auto cStr = m_string.c_str();
    delete m_rawString;
    m_rawString = new char[m_string.size() + 1];
    strcpy_s(m_rawString, m_string.size() + 1, cStr);
}
