#pragma once

#include "Editor/ImGuiWindows/Inspector.hpp"

namespace Sandbox
{
    class TransformInspector : public Inspector
    {
    public:
        TransformInspector();

        void OnGui() override;

        ImGuizmo::OPERATION currentGizmoOperation;
        ImGuizmo::MODE      currentGizmoMode;
        bool                useSnap;
        float               snap[3] = {1.f, 1.f, 1.f};
    };
}  // namespace Sandbox
