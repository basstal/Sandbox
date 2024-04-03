#pragma once

#include "Editor/IImGuiWindow.hpp"

namespace Sandbox {
class TransformInspector : public IImGuiWindow
{
  public:
    TransformInspector();

    void OnGui() override;

};
} // namespace Sandbox