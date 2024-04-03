#include "pch.hpp"

#include "DemoWindow.hpp"

Sandbox::DemoWindow::DemoWindow() { name = "DemoWindow"; }

void Sandbox::DemoWindow::OnGuiBegin()
{
    // override this function
    OnGui();
}

void Sandbox::DemoWindow::Prepare() {}

void Sandbox::DemoWindow::OnGui() { ImGui::ShowDemoWindow(&isOpen); }

void Sandbox::DemoWindow::Cleanup() {}
