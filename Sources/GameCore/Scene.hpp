#pragma once
#include <string>
#include <vector>

#include "Infrastructures/ISerializable.hpp"
#include "Persistence/ScenePersistence.hpp"

class GameObject;

class Scene : public ISerializable
{

    bool m_cleaned = false;
    bool m_started = false;
    ScenePersistence persistence;

public:
    std::vector<std::shared_ptr<GameObject>> gameObjects;
    std::string name = "default";

    Scene();

    ~Scene() override;

    void Update();

    void Cleanup();

    void Start();

    void SubmitToRender();

    void AddGameObject(std::shared_ptr<GameObject> gameObject);

    void Load() override;

    void Save() override;

    std::string GetSerializedPath() override;
};
