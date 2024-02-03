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
	uint32_t Width = 1920;
	uint32_t Height = 1080;
	std::string ApplicationName = "Sandbox";
	Settings();
	~Settings();
	void Save();
};
