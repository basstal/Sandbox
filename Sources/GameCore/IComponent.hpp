#pragma once
#include <memory>

class GameObject;

class IComponent
{
public:
	std::shared_ptr<GameObject> gameObject;
	virtual void Update() = 0;
	virtual ~IComponent() = default;
};
