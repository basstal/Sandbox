#pragma once
#include <memory>
#include <vector>

#include "Transform.hpp"

class IComponent;

class GameObject
{
public:
	std::shared_ptr<GameObject> parent;
	std::shared_ptr<Transform> transform;
	std::vector<std::shared_ptr<IComponent>> components;
	GameObject();
	~GameObject();
	void AddComponent(std::shared_ptr<IComponent> component);
	template <typename T>
	std::shared_ptr<T> GetComponent();
};

template <typename T>
std::shared_ptr<T> GameObject::GetComponent()
{
	for (auto& component : components)
	{
		if (std::dynamic_pointer_cast<T>(component))
		{
			return std::dynamic_pointer_cast<T>(component);
		}
	}
	return nullptr;
}
