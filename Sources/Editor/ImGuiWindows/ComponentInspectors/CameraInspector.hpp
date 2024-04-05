#pragma once

#include "Editor/IImGuiWindow.hpp"
#include "Editor/ImGuiWindows/Inspector.hpp"
#include "Generated/CameraInspector.rfkh.h"
#include "Serialization/Property/InspectComponentName.hpp"

namespace Sandbox NAMESPACE()
{
    class CLASS(InspectComponentName("Camera")) CameraInspector : public Inspector
    {
    public:
        CameraInspector();

        void OnInspectorGui() override;

        Sandbox_CameraInspector_GENERATED
    };

}  // namespace Sandbox NAMESPACE()
File_CameraInspector_GENERATED
