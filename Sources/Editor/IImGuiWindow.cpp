#include "IImGuiWindow.hpp"

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
