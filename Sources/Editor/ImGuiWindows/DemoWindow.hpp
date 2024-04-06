#pragma once
#include "Editor/IImGuiWindow.hpp"

namespace Sandbox
{
    class DemoWindow : public IImGuiWindow
    {
    public:
        DemoWindow();

        void OnGuiBegin() override;

        void Prepare() override;

        void OnGui() override;

        void Cleanup() override;

        bool showImGuiDemoWindow = true;
    };
}  // namespace Sandbox
