#pragma once

#include "Editor/IImGuiWindow.hpp"

namespace Sandbox
{
    class Logs : public IImGuiWindow
    {
    public:
        Logs();

        void AddLogMessage(const Sandbox::Logger::LogMessage& message);
        void AddLog(const std::string& message);

        void AddLog(const char* fmt, ...);

        void OnGui() override;

        ImGuiTextFilter filter;
        ImGuiTextBuffer stringBuilder;
        bool            autoScroll;
        ImVector<int>   lineOffsets;  // Index to lines offset. We maintain this with AddLog() calls.

        static std::shared_ptr<Logs> Instance;

    protected:
        void IncreaseLineNum(int oldSize);
        void Clear();
    };
}  // namespace Sandbox
