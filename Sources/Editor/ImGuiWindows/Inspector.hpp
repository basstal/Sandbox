#pragma once

#include "Editor/IImGuiWindow.hpp"
#include "Engine/EntityComponent/GameObject.hpp"

namespace Sandbox
{
    class Inspector : public IImGuiWindow
    {
    public:
        static std::map<std::string, std::shared_ptr<Sandbox::Inspector>> ComponentMapping;
                                                                          Inspector();

        void OnGui() override;

        void InspectTarget(std::shared_ptr<GameObject> inTarget);

    protected:
        std::shared_ptr<GameObject> m_target;
    };
}  // namespace Sandbox
