#include "Material.hpp"

#include "Renderer.hpp"
#include "CubeMap.hpp"
#include "Components/Pipeline.hpp"
#include "Infrastructures/SingletonOrganizer.hpp"

Material::~Material()
{
    // Cleanup();
}

Material::Material(const std::shared_ptr<Device>& device, const std::shared_ptr<GameCore::Image>& albedoImage, const std::shared_ptr<GameCore::Image>& metallicImage,
                   const std::shared_ptr<GameCore::Image>& roughnessImage,
                   const std::shared_ptr<GameCore::Image>& aoImage, const std::shared_ptr<GameCore::Image>& irradianceImage, const std::shared_ptr<CommandResource>& commandResource,
                   const std::shared_ptr<Pipeline>& pipeline, const std::shared_ptr<RenderPass>& renderPass, const std::shared_ptr<DescriptorResource>& descriptorResource)

{
    albedoMap = std::make_shared<Framebuffer>(device, commandResource, false, false, albedoImage->Width(), albedoImage->Height(), albedoImage->MipLevels());
    albedoMap->AssignImageData(albedoImage);

    metallicMap = std::make_shared<Framebuffer>(device, commandResource, false, false, metallicImage->Width(), metallicImage->Height(), metallicImage->MipLevels());
    metallicMap->AssignImageData(metallicImage);

    roughnessMap = std::make_shared<Framebuffer>(device, commandResource, false, false, roughnessImage->Width(), roughnessImage->Height(), roughnessImage->MipLevels());
    roughnessMap->AssignImageData(roughnessImage);

    aoMap = std::make_shared<Framebuffer>(device, commandResource, false, false, aoImage->Width(), aoImage->Height(), aoImage->MipLevels());
    aoMap->AssignImageData(aoImage);

    irradianceMap = std::make_shared<CubeMap>(device, irradianceImage, commandResource, irradianceMapSize, renderPass, descriptorResource);
    UpdateWriteDescriptorSet(pipeline->descriptorResource);
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

void Material::UpdateWriteDescriptorSet(const std::shared_ptr<DescriptorResource>& descriptorResource)
{
    auto device = SingletonOrganizer::Get<Renderer>()->device;
    for (size_t i = 0; i < Swapchain::MAX_FRAMES_IN_FLIGHT; i++)
    {
        int32_t bindingIrradiance = descriptorResource->nameToBinding.contains("irradianceMap") ? descriptorResource->nameToBinding["irradianceMap"] : -1;
        int32_t bindingTextures = descriptorResource->nameToBinding.contains("textures") ? descriptorResource->nameToBinding["textures"] : -1;
        if (bindingIrradiance >= 0)
        {
            VkDescriptorImageInfo irradianceImageInfo{};
            irradianceImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            irradianceImageInfo.imageView = irradianceMap->renderTexture->vkImageView;
            irradianceImageInfo.sampler = irradianceMap->renderTexture->vkSampler;

            VkWriteDescriptorSet irradianceMapWrite{};
            irradianceMapWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            irradianceMapWrite.dstSet = descriptorResource->vkDescriptorSets[i];
            irradianceMapWrite.dstBinding = bindingIrradiance;
            irradianceMapWrite.dstArrayElement = 0;
            irradianceMapWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            irradianceMapWrite.descriptorCount = 1;
            irradianceMapWrite.pImageInfo = &irradianceImageInfo;
            vkUpdateDescriptorSets(device->vkDevice, 1, &irradianceMapWrite, 0, nullptr);
        }

        if (bindingTextures >= 0)
        {
            std::array<VkDescriptorImageInfo, 4> pbrImageInfo{};
            pbrImageInfo[0].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            pbrImageInfo[0].imageView = albedoMap->vkImageView;
            pbrImageInfo[0].sampler = albedoMap->vkSampler;
            pbrImageInfo[1].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            pbrImageInfo[1].imageView = metallicMap->vkImageView;
            pbrImageInfo[1].sampler = metallicMap->vkSampler;
            pbrImageInfo[2].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            pbrImageInfo[2].imageView = roughnessMap->vkImageView;
            pbrImageInfo[2].sampler = roughnessMap->vkSampler;
            pbrImageInfo[3].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            pbrImageInfo[3].imageView = aoMap->vkImageView;
            pbrImageInfo[3].sampler = aoMap->vkSampler;

            // PBRMaterial
            VkWriteDescriptorSet pbrMaterialWrite{};
            pbrMaterialWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            pbrMaterialWrite.dstSet = descriptorResource->vkDescriptorSets[i];
            pbrMaterialWrite.dstBinding = bindingTextures;
            pbrMaterialWrite.dstArrayElement = 0;
            pbrMaterialWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            pbrMaterialWrite.descriptorCount = static_cast<uint32_t>(pbrImageInfo.size());
            pbrMaterialWrite.pImageInfo = pbrImageInfo.data();

            vkUpdateDescriptorSets(device->vkDevice, 1, &pbrMaterialWrite, 0, nullptr);
        }
    }
}
