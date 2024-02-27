#include "Settings.hpp"
#include "..\Supports\YamlGlmConverter.hpp"

#include <fstream>
#include <glm/vec3.hpp>

#include "Infrastructures/DataBinding.hpp"
#include "Infrastructures/Serialization.hpp"
#include "Infrastructures/FileSystem/FileSystemBase.hpp"

Settings::Settings()
{
	Settings::Load();
}

Settings::~Settings()
{
}

void Settings::Save()
{
	auto saveTo = GetSerializedPath();
	auto node = Serialization::SerializeToYaml(settingsConfig);
	std::ofstream fout(saveTo);
	fout << node;
}

void Settings::Load()
{
	auto loadFrom = GetSerializedPath();
	if (FileSystemBase::fileExists(loadFrom))
	{
		auto node = YAML::LoadFile(loadFrom);
		settingsConfig = Serialization::Deserialize<SettingsConfig>(node);
	}
}

std::string Settings::GetSerializedPath()
{
	std::string settingsDir = FileSystemBase::getSettingsDir();
	return settingsDir + "/rendering.yaml";
}

void Settings::UpdateEditorCamera(const std::shared_ptr<Camera>& camera)
{
	settingsConfig.EditorCameraPos = camera->position;
	settingsConfig.EditorCameraRotationX = camera->rotationX;
	settingsConfig.EditorCameraRotationZ = camera->rotationZ;
}
