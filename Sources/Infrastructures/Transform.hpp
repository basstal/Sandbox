#pragma once
#include <glm/fwd.hpp>
#include <glm/vec3.hpp>

#include "Math/Quaternion.hpp"

class Transform
{
public:
	glm::vec3 position;
	Quaternion rotation;
	glm::vec3 scale;

	glm::mat4 GetModelMatrix() const;
};
