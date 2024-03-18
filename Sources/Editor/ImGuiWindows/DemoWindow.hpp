#pragma once
#include "Editor/IImGuiWindow.hpp"

namespace Sandbox
{
    class DemoWindow : public IImGuiWindow
    {
    public:
        void OnGuiBegin() override;

        void Prepare() override;

        void OnGui() override;

        void Cleanup() override;

        bool showImGuiDemoWindow = true;
    };
}
