#pragma once

#include "Editor/ImGuiWindows/Inspector.hpp"
#include "Generated/MeshInspector.rfkh.h"
#include "Serialization/Property/InspectComponentName.hpp"

namespace Sandbox NAMESPACE()
{
    class CLASS(InspectComponentName("Mesh")) MeshInspector : public Inspector
    {
    public:
        MeshInspector();

        void OnInspectorGui() override;


        Sandbox_MeshInspector_GENERATED
    };
}  // namespace Sandbox NAMESPACE()

File_MeshInspector_GENERATED
