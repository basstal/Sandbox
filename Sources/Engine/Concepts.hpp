#pragma once


namespace Sandbox
{
    class IComponent;

    template <typename T>
    concept DerivedFromIComponent = std::is_base_of_v<IComponent, T>;

}  // namespace Sandbox
