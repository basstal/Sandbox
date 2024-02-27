#include "Settings.hpp"
#include "..\Supports\YamlGlmConverter.hpp"

#include <fstream>
#include <glm/vec3.hpp>

#include "Infrastructures/DataBinding.hpp"
#include "Infrastructures/FileSystem/FileSystemBase.hpp"

Settings::Settings()
{
	std::string settingsDir = FileSystemBase::getSettingsDir();
	m_renderingSettings = settingsDir + "/rendering.yaml";
	if (FileSystemBase::fileExists(m_renderingSettings))
	{
		config = YAML::LoadFile(m_renderingSettings);
	}
	try
	{
		Width = config["Width"].as<int>();
		Height = config["Height"].as<int>();
		WindowPositionX = config["WindowPositionX"].as<int>();
		WindowPositionY = config["WindowPositionY"].as<int>();
		ApplicationName = config["ApplicationName"].as<std::string>();
		IsWindow = config["IsWindow"].as<bool>();
		// FillModeNonSolid = config["FillModeNonSolid"].as<bool>();
		EditorCameraPos = config["EditorCameraPos"].as<glm::vec3>();
		EditorCameraRotationX = config["EditorCameraYaw"].as<float>();
		EditorCameraRotationZ = config["EditorCameraPitch"].as<float>();
		ViewMode = static_cast<EViewMode>(config["ViewMode"].as<int>());
	}
	catch (const std::exception&) // NOLINT(bugprone-empty-catch)
	{
	}
}

Settings::~Settings()
{
}


void Settings::Save()
{
	std::ofstream fout(m_renderingSettings);
	config["Width"] = Width;
	config["Height"] = Height;
	config["WindowPositionX"] = WindowPositionX;
	config["WindowPositionY"] = WindowPositionY;
	config["ApplicationName"] = ApplicationName;
	config["IsWindow"] = IsWindow;
	// config["FillModeNonSolid"] = FillModeNonSolid;
	config["EditorCameraPos"] = EditorCameraPos;
	config["EditorCameraYaw"] = EditorCameraRotationX;
	config["EditorCameraPitch"] = EditorCameraRotationZ;
	config["ViewMode"] = static_cast<int>(ViewMode);
	fout << config;
}

void Settings::UpdateEditorCamera(const std::shared_ptr<Camera>& camera)
{
	EditorCameraPos = camera->position;
	EditorCameraRotationX = camera->rotationX;
	EditorCameraRotationZ = camera->rotationZ;
}
