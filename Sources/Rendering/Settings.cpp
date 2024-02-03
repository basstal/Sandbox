#include "Settings.hpp"

#include <fstream>

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
		Width = config["Width"].as<uint32_t>();
		Height = config["Height"].as<uint32_t>();
		ApplicationName = config["ApplicationName"].as<std::string>();
	}
	catch (const std::exception&)
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
	config["ApplicationName"] = ApplicationName;
	fout << config;
}
