#include "Material.hpp"

#include "CubeMap.hpp"
Material::~Material()
{
	Cleanup();
}

Material::Material(const std::shared_ptr<Device>& device, const std::shared_ptr<Image>& albedoImage, const std::shared_ptr<Image>& metallicImage, const std::shared_ptr<Image>& roughnessImage,
                   const std::shared_ptr<Image>& aoImage, const std::shared_ptr<Image>& irradianceImage, const std::shared_ptr<CommandResource>& commandResource,
                   const std::shared_ptr<Pipeline>& pipeline, const std::shared_ptr<RenderPass>& renderPass, const std::shared_ptr<DescriptorResource>& descriptorResource)

{
	albedoMap = std::make_shared<RenderTexture>(device, commandResource, false, false, albedoImage->Width(), albedoImage->Height(), albedoImage->MipLevels());
	albedoMap->AssignImageData(albedoImage);

	metallicMap = std::make_shared<RenderTexture>(device, commandResource, false, false, metallicImage->Width(), metallicImage->Height(), metallicImage->MipLevels());
	metallicMap->AssignImageData(metallicImage);

	roughnessMap = std::make_shared<RenderTexture>(device, commandResource, false, false, roughnessImage->Width(), roughnessImage->Height(), roughnessImage->MipLevels());
	roughnessMap->AssignImageData(roughnessImage);

	aoMap = std::make_shared<RenderTexture>(device, commandResource, false, false, aoImage->Width(), aoImage->Height(), aoImage->MipLevels());
	aoMap->AssignImageData(aoImage);

	irradianceMap = std::make_shared<CubeMap>(device, irradianceImage, commandResource, irradianceMapSize, pipeline, renderPass, descriptorResource);
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
	if (irradianceMap != nullptr)
	{
		irradianceMap->Cleanup();
	}
}

void Material::TransitionImageLayout()
{
	// albedoMap->TransitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	// metallicMap->TransitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	// roughnessMap->TransitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	// aoMap->TransitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	// irradianceMap->renderTexture->TransitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	// irradianceMap->TransitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}
