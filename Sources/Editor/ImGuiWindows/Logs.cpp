#include "pch.hpp"

#include "Logs.hpp"
Sandbox::Logs::Logs() : autoScroll(true)
{
    name = "Logs";
    Clear();
    auto messages = Logger::messages;
    for (auto& message : messages)
    {
        AddLogMessage(message);
    }
    Logger::onLogMessage.BindMember<Logs, &Logs::AddLogMessage>(this);
}

void Sandbox::Logs::AddLogMessage(const Sandbox::Logger::LogMessage& message)
{
    auto [levelPrefix, _] = Logger::levelDetails[message.level];
    auto messageFormatted = std::vformat("{}{}{}\n", std::make_format_args(levelPrefix, message.tag, message.message));
    AddLog(messageFormatted);
}

void Sandbox::Logs::AddLog(const std::string& message)
{
    int oldSize = stringBuilder.size();
    stringBuilder.append(message.c_str());
    IncreaseLineNum(oldSize);
}

void Sandbox::Logs::AddLog(const char* fmt, ...)
{
    int     oldSize = stringBuilder.size();
    va_list args;
    va_start(args, fmt);
    stringBuilder.appendfv(fmt, args);
    va_end(args);
    IncreaseLineNum(oldSize);
}

void Sandbox::Logs::IncreaseLineNum(int oldSize)
{
    for (int newSize = stringBuilder.size(); oldSize < newSize; oldSize++)
        if (stringBuilder[oldSize] == '\n')
            lineOffsets.push_back(oldSize + 1);
}

void Sandbox::Logs::OnGui()
{
    // Options menu
    if (ImGui::BeginPopup("Options"))
    {
        ImGui::Checkbox("Auto-scroll", &autoScroll);
        ImGui::EndPopup();
    }

    // Main window
    if (ImGui::Button("Options"))
        ImGui::OpenPopup("Options");
    ImGui::SameLine();
    bool clear = ImGui::Button("Clear");
    ImGui::SameLine();
    bool copy = ImGui::Button("Copy");
    ImGui::SameLine();
    filter.Draw("Filter", -100.0f);

    ImGui::Separator();

    if (ImGui::BeginChild("scrolling", ImVec2(0, 0), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar))
    {
        if (clear)
            Clear();
        if (copy)
            ImGui::LogToClipboard();

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        const char* buf     = stringBuilder.begin();
        const char* buf_end = stringBuilder.end();
        if (filter.IsActive())
        {
            // In this example we don't use the clipper when Filter is enabled.
            // This is because we don't have random access to the result of our filter.
            // A real application processing logs with ten of thousands of entries may want to store the result of
            // search/filter.. especially if the filtering function is not trivial (e.g. reg-exp).
            for (int line_no = 0; line_no < lineOffsets.Size; line_no++)
            {
                const char* line_start = buf + lineOffsets[line_no];
                const char* line_end   = (line_no + 1 < lineOffsets.Size) ? (buf + lineOffsets[line_no + 1] - 1) : buf_end;
                if (filter.PassFilter(line_start, line_end))
                    ImGui::TextUnformatted(line_start, line_end);
            }
        }
        else
        {
            // The simplest and easy way to display the entire buffer:
            //   ImGui::TextUnformatted(buf_begin, buf_end);
            // And it'll just work. TextUnformatted() has specialization for large blob of text and will fast-forward
            // to skip non-visible lines. Here we instead demonstrate using the clipper to only process lines that are
            // within the visible area.
            // If you have tens of thousands of items and their processing cost is non-negligible, coarse clipping them
            // on your side is recommended. Using ImGuiListClipper requires
            // - A) random access into your data
            // - B) items all being the  same height,
            // both of which we can handle since we have an array pointing to the beginning of each line of text.
            // When using the filter (in the block of code above) we don't have random access into the data to display
            // anymore, which is why we don't use the clipper. Storing or skimming through the search result would make
            // it possible (and would be recommended if you want to search through tens of thousands of entries).
            ImGuiListClipper clipper;
            clipper.Begin(lineOffsets.Size);
            while (clipper.Step())
            {
                for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
                {
                    const char* line_start = buf + lineOffsets[line_no];
                    const char* line_end   = (line_no + 1 < lineOffsets.Size) ? (buf + lineOffsets[line_no + 1] - 1) : buf_end;
                    ImGui::TextUnformatted(line_start, line_end);
                }
            }
            clipper.End();
        }
        ImGui::PopStyleVar();

        // Keep up at the bottom of the scroll region if we were already at the bottom at the beginning of the frame.
        // Using a scrollbar or mouse-wheel will take away from the bottom edge.
        if (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
            ImGui::SetScrollHereY(1.0f);
    }
    ImGui::EndChild();
}
void Sandbox::Logs::Clear()
{
    stringBuilder.clear();
    lineOffsets.clear();
    lineOffsets.push_back(0);
}
