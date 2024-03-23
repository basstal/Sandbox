#include "Scene.hpp"

std::shared_ptr<Sandbox::Scene> Sandbox::Scene::currentScene = nullptr;

std::shared_ptr<Sandbox::Scene> Sandbox::Scene::GetCurrentScene()
{
    return currentScene;
}
