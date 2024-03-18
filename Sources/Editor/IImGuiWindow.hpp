#pragma once
#include <string>

#include "imgui.h"


namespace Sandbox
{
    class IImGuiWindow
    {
    public:
        std::string name = "DEFAULT";
        bool isOpen = false;
        ImGuiWindowFlags flags = 0;

        virtual ~IImGuiWindow() = default;

        virtual void Prepare() = 0;

        virtual void OnGuiBegin();

        virtual void OnGui() = 0;

        virtual void Tick(float deltaTime);

        virtual void Cleanup() = 0;
    };
}
