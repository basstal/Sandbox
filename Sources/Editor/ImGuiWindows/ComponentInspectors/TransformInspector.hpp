#pragma once

#include <ImGuizmo.h>

#include "Editor/ImGuiWindows/Inspector.hpp"
#include "Generated/TransformInspector.rfkh.h"
#include "Serialization/Property/InspectComponentName.hpp"

namespace Sandbox NAMESPACE()
{
    class CLASS(InspectComponentName("Transform")) TransformInspector : public Inspector
    {
    public:
        TransformInspector();

        void OnInspectorGui() override;

        Sandbox_TransformInspector_GENERATED
    };
}  // namespace Sandbox NAMESPACE()

File_TransformInspector_GENERATED
