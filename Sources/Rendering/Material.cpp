#include "Material.hpp"
Material::~Material()
{
	Cleanup();
}

Material::Material(const std::shared_ptr<Device>& device, const std::shared_ptr<Image>& albedoImage, const std::shared_ptr<Image>& metallicImage, const std::shared_ptr<Image>& roughnessImage,
                   const std::shared_ptr<Image>& aoImage, const std::shared_ptr<CommandResource>& commandResource)

{
	albedoMap = std::make_shared<RenderTexture>(device, albedoImage, commandResource);
	metallicMap = std::make_shared<RenderTexture>(device, metallicImage, commandResource);
	roughnessMap = std::make_shared<RenderTexture>(device, roughnessImage, commandResource);
	aoMap = std::make_shared<RenderTexture>(device, aoImage, commandResource);
}

void Material::Cleanup()
{
	if (m_cleaned)
	{
		return;
	}
	if (albedoMap != nullptr)
	{
		albedoMap->Cleanup();
	}
	if (metallicMap != nullptr)
	{
		metallicMap->Cleanup();
	}
	if (roughnessMap != nullptr)
	{
		roughnessMap->Cleanup();
	}
	if (aoMap != nullptr)
	{
		aoMap->Cleanup();
	}
}
