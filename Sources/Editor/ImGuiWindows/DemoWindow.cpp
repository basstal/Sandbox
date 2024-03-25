#include "pch.hpp"

#include "DemoWindow.hpp"


void Sandbox::DemoWindow::OnGuiBegin()
{
    // override this function
    OnGui();
}

void Sandbox::DemoWindow::Prepare()
{
    name = "DemoWindow";
}

void Sandbox::DemoWindow::OnGui()
{
    ImGui::ShowDemoWindow(&isOpen);
}

void Sandbox::DemoWindow::Cleanup()
{
}
