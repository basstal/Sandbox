#pragma once
#include <cstdint>
#include <string>
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
	std::string ApplicationName = "Sandbox";
	Settings();
	~Settings();
	void Save();
};
