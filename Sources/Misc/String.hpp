#pragma once
#include <string>
#include <vector>

#include "Generated/String.rfkh.h"
#include "Serialization/ISerializable.hpp"

namespace Sandbox NAMESPACE()
{
    class CLASS() String : public ISerializable<String>
    {
    public:
        FIELD()
        char* rawString = nullptr;

        String() = default;

        String(const std::string& string);

        String(const char* string);

        String(const String& other);

        String(String&& other) noexcept;

        String& operator=(const char* inString);

        static std::vector<std::string> Split(const std::string& source, const char& seperator);

        static std::string Replace(const std::string& source, const std::string& from, const std::string& to);

        std::string ToStdString();

        METHOD()
        void Construct(const std::string& inString);

    private:
        std::string m_string;
        void        Sync();
        Sandbox_String_GENERATED
    };
}  // namespace Sandbox NAMESPACE()

File_String_GENERATED
