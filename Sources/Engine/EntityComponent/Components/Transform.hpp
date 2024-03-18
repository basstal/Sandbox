#pragma once

#include "glm/fwd.hpp"
#include "glm/detail/type_quat.hpp"
#include "Engine/EntityComponent/IComponent.hpp"

namespace Sandbox
{
    class Transform : public IComponent
    {
    public:
        glm::vec3 position;

        glm::quat rotation = glm::quat(1, 0, 0, 0);

        glm::vec3 scale = glm::vec3(1.0f);

        void Cleanup() override;

        glm::mat4 GetModelMatrix() const;

        glm::mat4 GetModelTranslate() const;
    };
}
