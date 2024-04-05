#pragma once

#include "Editor/IImGuiWindow.hpp"

namespace Sandbox
{
    class Stats : public IImGuiWindow
    {
    public:
        Stats();

        void OnGui() override;
    };
}  // namespace Sandbox
