#pragma once

#include <Refureku/Properties/PropertySettings.h>

#include "Generated/InspectComponentName.rfkh.h"

namespace Sandbox NAMESPACE()
{
    class CLASS(rfk::PropertySettings(rfk::EEntityKind::Struct | rfk::EEntityKind::Class)) InspectComponentName : public rfk::Property
    {
    public:
        std::string name;

        InspectComponentName(const std::string& inName);

        Sandbox_InspectComponentName_GENERATED
    };
}  // namespace Sandbox NAMESPACE()

File_InspectComponentName_GENERATED
