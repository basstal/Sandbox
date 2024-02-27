#pragma once
#include <glm/fwd.hpp>
#include <glm/vec3.hpp>

#include "Persistence/TransformPersistence.hpp"

class Transform
{
private:
	TransformPersistence serializable;

public:
	glm::vec3 GetPosition() const;
	glm::mat4 GetModelMatrix() const;
};
