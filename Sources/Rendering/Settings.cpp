#include "Settings.hpp"

#include <fstream>

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
	fout << config;
}
