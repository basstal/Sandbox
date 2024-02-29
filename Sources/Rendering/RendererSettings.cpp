#include "RendererSettings.hpp"

#include <fstream>

#include "Infrastructures/DataBinding.hpp"
#include "Infrastructures/Serialization.hpp"
#include "Infrastructures/FileSystem/FileSystemBase.hpp"
#include "Rendering/Persistence/CameraPersistence.hpp"

RendererSettings::RendererSettings()
{
    RendererSettings::Load();
    persistence.editorCamera = persistence.editorCamera == nullptr ? std::make_shared<CameraPersistence>() : persistence.editorCamera;
}

RendererSettings::~RendererSettings()
{
    RendererSettings::Save();
}

void RendererSettings::Save()
{
    auto saveTo = GetSerializedPath();
    auto node = Serialization::SerializeToYaml(persistence);
    std::ofstream fout(saveTo);
    fout << node;
}

void RendererSettings::Load()
{
    auto loadFrom = GetSerializedPath();
    if (FileSystemBase::fileExists(loadFrom))
    {
        auto node = YAML::LoadFile(loadFrom);
        persistence = Serialization::Deserialize<RendererPersistence>(node);
    }
}

std::string RendererSettings::GetSerializedPath()
{
    std::string settingsDir = FileSystemBase::getSettingsDir();
    return settingsDir + "/rendering.yaml";
}
