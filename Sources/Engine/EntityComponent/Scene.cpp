#include "pch.hpp"

#include "Scene.hpp"
#include "Generated/Scene.rfks.h"

std::shared_ptr<Sandbox::Scene> Sandbox::Scene::currentScene = nullptr;

std::shared_ptr<Sandbox::Scene> Sandbox::Scene::GetCurrentScene()
{
    return currentScene;
}

void Sandbox::Scene::Cleanup() {
    if(m_cleaned)
    {
        return;
    }
    m_cleaned = true;
    for(auto& gameObject : rootGameObjects)
    {
        gameObject->Cleanup();
    }
}
