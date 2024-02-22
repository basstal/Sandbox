#include "GameObject.hpp"
GameObject::GameObject()
{
	transform = std::make_shared<Transform>();
}
GameObject::~GameObject()
{
}
