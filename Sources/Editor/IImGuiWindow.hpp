#pragma once
#include <Generated/IImGuiWindow.rfkh.h>
#include <imgui.h>
#include <string>

#include "Serialization/ISerializable.hpp"

namespace Sandbox NAMESPACE() {
class CLASS() IImGuiWindow : public ISerializable<IImGuiWindow>
{
  public:
    std::string name = "DEFAULT";
    FIELD()
    bool isOpen = false;
    ImGuiWindowFlags flags = 0;

    virtual ~IImGuiWindow() = default;

    virtual void Prepare();

    virtual void OnGuiBegin();

    virtual void OnGui() = 0;

    virtual void Tick(float deltaTime);

    virtual void Cleanup();

    virtual File GetConfigCachePath() const;

    Sandbox_IImGuiWindow_GENERATED
};
} // namespace Sandbox NAMESPACE()

File_IImGuiWindow_GENERATED
