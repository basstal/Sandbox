#pragma once
#include <memory>

#include "Transform.hpp"
#include "Rendering/Model.hpp"

class GameObject
{
public:
	std::shared_ptr<Transform> transform;
	std::shared_ptr<Model> model;
	GameObject();
	~GameObject();
};
