#pragma once
#include <string>
#include <vector>

namespace Sandbox
{
    class String
    {
    public:
        static std::vector<std::string> Split(const std::string& source, const char& seperator);

        static std::string Replace(const std::string& source, const std::string& from, const std::string& to);
    };
}
