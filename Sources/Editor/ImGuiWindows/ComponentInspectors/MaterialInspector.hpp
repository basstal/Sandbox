#pragma once

#include "Editor/ImGuiWindows/Inspector.hpp"
#include "Generated/MaterialInspector.rfkh.h"
#include "Serialization/Property/InspectComponentName.hpp"

namespace Sandbox NAMESPACE()
{
    class CLASS(InspectComponentName("Material")) MaterialInspector : public Inspector
    {
    public:

        void OnInspectorGui() override;
        bool InspectorProperty(String& property, const std::string& label);


        Sandbox_MaterialInspector_GENERATED
    };
}  // namespace Sandbox NAMESPACE()

File_MaterialInspector_GENERATED
