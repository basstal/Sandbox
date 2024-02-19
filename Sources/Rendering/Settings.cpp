#include "Settings.hpp"
#include "Supports/YamlWithGlm.hpp"

#include <fstream>
#include <glm/vec3.hpp>

#include "Infrastructures/DataBinding.hpp"
#include "Infrastructures/FileSystemBase.hpp"

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
		FillModeNonSolid = config["FillModeNonSolid"].as<bool>();
		EditorCameraPos = config["EditorCameraPos"].as<glm::vec3>();
		EditorCameraUp = config["EditorCameraUp"].as<glm::vec3>();
		EditorCameraYaw = config["EditorCameraYaw"].as<float>();
		EditorCameraPitch = config["EditorCameraPitch"].as<float>();
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
	config["FillModeNonSolid"] = FillModeNonSolid;
	config["EditorCameraPos"] = EditorCameraPos;
	config["EditorCameraUp"] = EditorCameraUp;
	config["EditorCameraYaw"] = EditorCameraYaw;
	config["EditorCameraPitch"] = EditorCameraPitch;
	fout << config;
}

void Settings::UpdateEditorCamera(const std::shared_ptr<Camera>& camera)
{
	EditorCameraPos = camera->Position;
	EditorCameraUp = camera->Up;
	EditorCameraYaw = camera->Yaw;
	EditorCameraPitch = camera->Pitch;
}
