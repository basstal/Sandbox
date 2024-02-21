#pragma once
#include <memory>
#include <string>
#include <glm/vec3.hpp>
#include "Camera.hpp"
#include "ViewMode.hpp"
#include "yaml-cpp/yaml.h"

class Settings
{
private:
	std::string m_renderingSettings;
	YAML::Node config;

public:
	bool IsWindow = false;
	int WindowPositionX = 0;
	int WindowPositionY = 0;
	int Width = 1920;
	int Height = 1080;
	// bool FillModeNonSolid = false;
	glm::vec3 EditorCameraPos = glm::vec3(0.0f);
	float EditorCameraRotationX = DEFAULT_ROTATION_X;
	float EditorCameraRotationZ = DEFAULT_ROTATION_Z;
	std::string ApplicationName = "Sandbox";
	EViewMode ViewMode = EViewMode::Lit;
	Settings();
	~Settings();
	void Save();
	void UpdateEditorCamera(const std::shared_ptr<Camera>& camera);
};
