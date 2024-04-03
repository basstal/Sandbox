#pragma once

#include <glm/detail/type_quat.hpp>
#include <glm/fwd.hpp>

#include "Engine/EntityComponent/IComponent.hpp"
#include "Generated/Transform.rfkh.h"
#include "Math/Quaternion.hpp"
#include "Math/Vector3.hpp"

namespace Sandbox NAMESPACE() {
class CLASS() Transform : public IComponent, public ISerializable<Transform>
{
  public:
    FIELD()
    Vector3 position;

    FIELD()
    Quaternion rotation = glm::quat(1, 0, 0, 0);

    FIELD()
    Vector3 scale = glm::vec3(1.0f);

    void Cleanup() override;

    glm::mat4 GetModelMatrix();

    glm::mat4 GetModelTranslate();

    Sandbox_Transform_GENERATED
};
} // namespace Sandbox NAMESPACE()

File_Transform_GENERATED
