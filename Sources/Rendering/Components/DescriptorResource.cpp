#include "DescriptorResource.hpp"

#include <array>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

#include "Rendering/Camera.hpp"
#include "Rendering/Light.hpp"
#include "Rendering/Material.hpp"
#include "Rendering/Base/Device.hpp"
#include "Rendering/Objects/RenderTexture.hpp"
#include "Rendering/Objects/UniformBuffers.hpp"


DescriptorResource::DescriptorResource(const std::shared_ptr<Device>& device):
	vkDescriptorPool(nullptr)
{
	m_device = device;

	VkDescriptorSetLayoutBinding mvpLayoutBinding;
	mvpLayoutBinding.binding = 0;
	mvpLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	mvpLayoutBinding.descriptorCount = 1;
	mvpLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	mvpLayoutBinding.pImmutableSamplers = nullptr; // Optional

	// VkDescriptorSetLayoutBinding samplerLayoutBinding;
	// samplerLayoutBinding.binding = 1;
	// samplerLayoutBinding.descriptorCount = 1;
	// samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	// samplerLayoutBinding.pImmutableSamplers = nullptr;
	// samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding pbrMaterialLayoutBinding;
	pbrMaterialLayoutBinding.binding = 1;
	pbrMaterialLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	pbrMaterialLayoutBinding.descriptorCount = 4;
	pbrMaterialLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	pbrMaterialLayoutBinding.pImmutableSamplers = nullptr; // Optional


	VkDescriptorSetLayoutBinding pbrLightLayoutBinding;
	pbrLightLayoutBinding.binding = 2;
	pbrLightLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	pbrLightLayoutBinding.descriptorCount = 1;
	pbrLightLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	pbrLightLayoutBinding.pImmutableSamplers = nullptr; // Optional

	VkDescriptorSetLayoutBinding irradianceMapLayoutBinding;
	irradianceMapLayoutBinding.binding = 3;
	irradianceMapLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	irradianceMapLayoutBinding.descriptorCount = 1;
	irradianceMapLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	irradianceMapLayoutBinding.pImmutableSamplers = nullptr; // Optional

	vkDescriptorSetLayout = CreateDescriptorSetLayout({mvpLayoutBinding, pbrMaterialLayoutBinding, pbrLightLayoutBinding, irradianceMapLayoutBinding});
}

DescriptorResource::~DescriptorResource()
{
	Cleanup();
}

VkDescriptorSetLayout DescriptorResource::CreateDescriptorSetLayout(const std::vector<VkDescriptorSetLayoutBinding>& bindings)
{
	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();

	VkDescriptorSetLayout layout;
	if (vkCreateDescriptorSetLayout(m_device->vkDevice, &layoutInfo, nullptr, &layout) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create descriptor set layout!");
	}
	return layout;
}

void DescriptorResource::CreateDescriptorPool()
{
	std::array<VkDescriptorPoolSize, 2> poolSizes{};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = static_cast<uint32_t>(DESCRIPTOR_POOL_SIZE);
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = static_cast<uint32_t>(DESCRIPTOR_POOL_SIZE);

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = DESCRIPTOR_POOL_SIZE;
	if (vkCreateDescriptorPool(m_device->vkDevice, &poolInfo, nullptr, &vkDescriptorPool) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create descriptor pool!");
	}
}

void DescriptorResource::CreateDescriptorSets(const std::shared_ptr<UniformBuffers>& uniformBuffers, const std::shared_ptr<RenderTexture>& renderTexture, const std::shared_ptr<Material>& material)
{
	std::vector<VkDescriptorSetLayout> layouts(m_device->MAX_FRAMES_IN_FLIGHT, vkDescriptorSetLayout);
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = vkDescriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(m_device->MAX_FRAMES_IN_FLIGHT);
	allocInfo.pSetLayouts = layouts.data();
	vkDescriptorSets.resize(m_device->MAX_FRAMES_IN_FLIGHT);
	if (vkAllocateDescriptorSets(m_device->vkDevice, &allocInfo, vkDescriptorSets.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate descriptor sets!");
	}
	for (size_t i = 0; i < m_device->MAX_FRAMES_IN_FLIGHT; i++)
	{
		VkDescriptorBufferInfo mvpBufferInfo{};
		mvpBufferInfo.buffer = uniformBuffers->mvpObjectBuffers[i]->vkBuffer;
		mvpBufferInfo.offset = 0;
		mvpBufferInfo.range = sizeof(MVPObject);

		// VkDescriptorBufferInfo pbrMaterialBufferInfo{};
		// pbrMaterialBufferInfo.buffer = uniformBuffers->pbrMaterialBuffers[i]->vkBuffer; // PBRMaterial 的缓冲区
		// pbrMaterialBufferInfo.offset = 0;
		// pbrMaterialBufferInfo.range = sizeof(PBRMaterial);

		VkDescriptorBufferInfo pbrLightBufferInfo{};
		pbrLightBufferInfo.buffer = uniformBuffers->pbrLightBuffers[i]->vkBuffer; // PRBLight 的缓冲区
		pbrLightBufferInfo.offset = 0;
		pbrLightBufferInfo.range = sizeof(Light);


		// VkDescriptorImageInfo imageInfo{};
		// imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		// imageInfo.imageView = renderTexture->vkImageView;
		// imageInfo.sampler = renderTexture->vkSampler;
		// descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		// descriptorWrites[1].dstSet = vkDescriptorSets[i];
		// descriptorWrites[1].dstBinding = 1;
		// descriptorWrites[1].dstArrayElement = 0;
		// descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		// descriptorWrites[1].descriptorCount = 1;
		// descriptorWrites[1].pImageInfo = &imageInfo;

		std::array<VkDescriptorImageInfo, 4> pbrImageInfo{};
		pbrImageInfo[0].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		pbrImageInfo[0].imageView = material->albedoMap->vkImageView;
		pbrImageInfo[0].sampler = material->albedoMap->vkSampler;
		pbrImageInfo[1].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		pbrImageInfo[1].imageView = material->metallicMap->vkImageView;
		pbrImageInfo[1].sampler = material->metallicMap->vkSampler;
		pbrImageInfo[2].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		pbrImageInfo[2].imageView = material->roughnessMap->vkImageView;
		pbrImageInfo[2].sampler = material->roughnessMap->vkSampler;
		pbrImageInfo[3].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		pbrImageInfo[3].imageView = material->aoMap->vkImageView;
		pbrImageInfo[3].sampler = material->aoMap->vkSampler;

		VkWriteDescriptorSet mvpWrite{};
		mvpWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		mvpWrite.dstSet = vkDescriptorSets[i];
		mvpWrite.dstBinding = 0;
		mvpWrite.dstArrayElement = 0;
		mvpWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		mvpWrite.descriptorCount = 1;
		mvpWrite.pBufferInfo = &mvpBufferInfo;
		mvpWrite.pImageInfo = nullptr; // Optional
		mvpWrite.pTexelBufferView = nullptr; // Optional
		
		// PBRMaterial
		VkWriteDescriptorSet pbrMaterialWrite{};
		pbrMaterialWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		pbrMaterialWrite.dstSet = vkDescriptorSets[i]; // 您的描述符集
		pbrMaterialWrite.dstBinding = 1; // 对应于 layout(binding = 1) in GLSL
		pbrMaterialWrite.dstArrayElement = 0;
		pbrMaterialWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		pbrMaterialWrite.descriptorCount = static_cast<uint32_t>(pbrImageInfo.size());
		pbrMaterialWrite.pImageInfo = pbrImageInfo.data();

		// PRBLight
		VkWriteDescriptorSet pbrLightWrite{};
		pbrLightWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		pbrLightWrite.dstSet = vkDescriptorSets[i]; // 您的描述符集
		pbrLightWrite.dstBinding = 2; // 对应于 layout(binding = 2) in GLSL
		pbrLightWrite.dstArrayElement = 0;
		pbrLightWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		pbrLightWrite.descriptorCount = 1;
		pbrLightWrite.pBufferInfo = &pbrLightBufferInfo;

		VkDescriptorImageInfo irradianceImageInfo{};
		irradianceImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		irradianceImageInfo.imageView = material->irradianceMap->renderTexture->vkImageView;
		irradianceImageInfo.sampler = material->irradianceMap->renderTexture->vkSampler;

		VkWriteDescriptorSet irradianceMapWrite{};
		irradianceMapWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		irradianceMapWrite.dstSet = vkDescriptorSets[i];
		irradianceMapWrite.dstBinding = 3;
		irradianceMapWrite.dstArrayElement = 0;
		irradianceMapWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		irradianceMapWrite.descriptorCount = 1;
		irradianceMapWrite.pImageInfo = &irradianceImageInfo;

		std::array descriptorWrites{mvpWrite, pbrMaterialWrite, pbrLightWrite, irradianceMapWrite};
		vkUpdateDescriptorSets(m_device->vkDevice, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}
}

void DescriptorResource::Cleanup()
{
	if (!m_cleaned)
	{
		vkDestroyDescriptorPool(m_device->vkDevice, vkDescriptorPool, nullptr);
		vkDestroyDescriptorSetLayout(m_device->vkDevice, vkDescriptorSetLayout, nullptr);
		m_cleaned = true;
	}
}
