#pragma once
#include <glm/fwd.hpp>
#include <glm/vec3.hpp>

#include "Infrastructures/Math/Quaternion.hpp"

class Transform
{
public:
	glm::vec3 position = glm::vec3(0.0f);
	Quaternion rotation = Quaternion::identity;
	glm::vec3 scale = glm::vec3(1.0f);

	glm::mat4 GetModelMatrix() const;
};
