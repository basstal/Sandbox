#pragma once

#include "Editor/IImGuiWindow.hpp"
#include "Engine/EntityComponent/GameObject.hpp"

namespace Sandbox {
class Inspector : public IImGuiWindow
{
  public:
    Inspector();

    void OnGui() override;

    std::shared_ptr<GameObject> target;
};
} // namespace Sandbox