#pragma once

namespace Sandbox
{
    class IImGuiWindow;

    class MenuBar
    {
    public:
        bool Draw(const std::vector<std::shared_ptr<IImGuiWindow>>& windows);
    };
}  // namespace Sandbox
