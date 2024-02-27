#pragma once
#include <memory>
#include <string>
#include <glm/vec3.hpp>
#include "Camera.hpp"
#include "ViewMode.hpp"
#include "yaml-cpp/yaml.h"
#include <boost/hana.hpp>

#include "Infrastructures/ISerializable.hpp"

struct SettingsConfig
{
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
};

class Settings : public ISerializable
{
private:
	SettingsConfig persistence; // TODO: migrate to this

public:
	SettingsConfig settingsConfig;
	// bool IsWindow = false;
	// int WindowPositionX = 0;
	// int WindowPositionY = 0;
	// int Width = 1920;
	// int Height = 1080;
	// // bool FillModeNonSolid = false;
	// glm::vec3 EditorCameraPos = glm::vec3(0.0f);
	// float EditorCameraRotationX = DEFAULT_ROTATION_X;
	// float EditorCameraRotationZ = DEFAULT_ROTATION_Z;
	// std::string ApplicationName = "Sandbox";
	// EViewMode ViewMode = EViewMode::Lit;
	Settings();
	~Settings();
	void Save() override;
	void Load() override;
	std::string GetSerializedPath() override;
	void UpdateEditorCamera(const std::shared_ptr<Camera>& camera);
};

// 使 Settings 与 Boost.Hana 兼容
BOOST_HANA_ADAPT_STRUCT(SettingsConfig, IsWindow, WindowPositionX, WindowPositionY, Width, Height, EditorCameraPos, EditorCameraRotationX, EditorCameraRotationZ, ApplicationName, ViewMode);
