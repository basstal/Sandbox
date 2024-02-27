#include "GameObject.hpp"

#include "IComponent.hpp"
GameObject::GameObject()
{
	transform = std::make_shared<Transform>();
}
GameObject::~GameObject()
{
}
void GameObject::AddComponent(std::shared_ptr<IComponent> component)
{
	components.push_back(component);
}
