#include "pch.hpp"

#include "MenuBar.hpp"

#include "Editor/IImGuiWindow.hpp"
#include "Editor/ImGuiExamples.hpp"
#include "Misc/DataBinding.hpp"
bool Sandbox::MenuBar::Draw(const std::vector<std::shared_ptr<IImGuiWindow>>& windows)
{
    // 绘制菜单栏
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            ShowExampleMenuFile();
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Undo", "CTRL+Z"))
            {
            }
            if (ImGui::MenuItem("Redo", "CTRL+Y", false, false))
            {
            }  // Disabled item
            ImGui::Separator();
            if (ImGui::MenuItem("Cut", "CTRL+X"))
            {
            }
            if (ImGui::MenuItem("Copy", "CTRL+C"))
            {
            }
            if (ImGui::MenuItem("Paste", "CTRL+V"))
            {
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Windows"))
        {
            for (auto& window : windows)
            {
                ImGui::Checkbox(window->name.c_str(), &window->isOpen);
                // LOGD("window->isOpen : {}", std::to_string(window->isOpen))
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Tests"))
        {
            if (ImGui::MenuItem("CreateRenderer"))
            {
                LOGD("CreateRenderer Clicked")
                DataBinding::Get<void>("Engine/Reload")->Trigger();
            }

            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
    return true;
}
