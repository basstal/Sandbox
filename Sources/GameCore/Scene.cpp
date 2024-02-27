#include "Scene.hpp"

#include <fstream>

#include "IComponent.hpp"
#include "GameObject.hpp"
#include "Infrastructures/Serialization.hpp"
#include "Infrastructures/FileSystem/File.hpp"
#include "Infrastructures/FileSystem/FileSystemBase.hpp"
#include "Persistence/GameObjectPersistence.hpp"
#include "Persistence/ScenePersistence.hpp"
#include "yaml-cpp/node/parse.h"
#include "yaml-cpp/yaml.h"
#include "Supports/YamlGlmConverter.hpp"

Scene::Scene()
{
}
Scene::~Scene()
{
	Scene::Save();
}
void Scene::Update()
{
	if (!m_started)
	{
		Start();
		return;
	}
	for (auto& gameObject : gameObjects)
	{
		for (auto& component : gameObject->components)
		{
			component->Update();
		}
	}
}


void Scene::Start()
{
}

void Scene::AddGameObject(std::shared_ptr<GameObject> gameObject)
{
	gameObjects.push_back(gameObject);
	persistence.gameObjects.push_back(GameObjectPersistence());
}

void Scene::Load()
{
	auto loadFrom = GetSerializedPath();
	if (FileSystemBase::fileExists(loadFrom))
	{
		auto node = YAML::LoadFile(loadFrom);
		persistence = Serialization::Deserialize<ScenePersistence>(node);
	}
}

void Scene::Save()
{
	auto saveTo = GetSerializedPath();
	auto node = Serialization::SerializeToYaml(persistence);
	std::ofstream fout(saveTo);
	fout << node;
}

std::string Scene::GetSerializedPath()
{
	std::string settingsDir = FileSystemBase::getSettingsDir();
	File file(settingsDir + "/Scene/" + name + ".yaml");
	file.CreateDirectory();
	return file.GetPath();
}
