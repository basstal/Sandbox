#include "IImGuiWindow.hpp"

#include "FileSystem/Directory.hpp"
#include "Generated/IImGuiWindow.rfks.h"

void Sandbox::IImGuiWindow::Prepare()
{
    LoadFromFile(GetConfigCachePath());
}


void Sandbox::IImGuiWindow::OnGuiBegin()
{
    if (!isOpen)
    {
        return;
    }
    if (!ImGui::Begin(name.c_str(), &isOpen, flags))
    {
        ImGui::End();
    }
    else
    {
        OnGui();
        ImGui::End();
    }
}

void Sandbox::IImGuiWindow::Tick(float deltaTime)
{
}

void Sandbox::IImGuiWindow::Cleanup()
{
    SaveToFile(GetConfigCachePath());
}

Sandbox::File Sandbox::IImGuiWindow::GetConfigCachePath() const
{
    return Sandbox::Directory::GetLibraryDirectory().GetFile(std::format("{}ConfigCache.yaml", name));
}
