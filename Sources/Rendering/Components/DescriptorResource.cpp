#include "DescriptorResource.hpp"

#include <array>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

#include "Swapchain.hpp"
#include "Infrastructures/FileSystem/Logger.hpp"
#include "Rendering/Camera.hpp"
#include "Rendering/Light.hpp"
#include "Rendering/Material.hpp"
#include "Rendering/Base/Device.hpp"
#include "Rendering/Objects/Framebuffer.hpp"
#include "Rendering/Buffers/UniformBuffers.hpp"
#include "Rendering/Objects/Shader.hpp"

VkDescriptorPool DescriptorResource::vkDescriptorPool = nullptr;

DescriptorResource::DescriptorResource(const std::shared_ptr<Device>& device)
{
	m_device = device;
	// CreateDescriptorPool();
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
		Logger::Fatal("failed to create descriptor set layout!");
	}
	return layout;
}

void DescriptorResource::CreateDescriptorPool(const std::shared_ptr<Device>& device)
{
	if (vkDescriptorPool != nullptr)
	{
		return;
	}
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
	if (vkCreateDescriptorPool(device->vkDevice, &poolInfo, nullptr, &vkDescriptorPool) != VK_SUCCESS)
	{
		Logger::Fatal("failed to create descriptor pool!");
	}
}

void DescriptorResource::CreateDescriptorSets()
{
	std::vector<VkDescriptorSetLayout> layouts(Swapchain::MAX_FRAMES_IN_FLIGHT, vkDescriptorSetLayout);
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = vkDescriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(Swapchain::MAX_FRAMES_IN_FLIGHT);
	allocInfo.pSetLayouts = layouts.data();
	vkDescriptorSets.resize(Swapchain::MAX_FRAMES_IN_FLIGHT);
	if (vkAllocateDescriptorSets(m_device->vkDevice, &allocInfo, vkDescriptorSets.data()) != VK_SUCCESS)
	{
		Logger::Fatal("failed to allocate descriptor sets!");
	}
}


void DescriptorResource::Cleanup()
{
	if (!m_cleaned)
	{
		// TODO: recover the following code
		// vkDestroyDescriptorPool(m_device->vkDevice, vkDescriptorPool, nullptr);
		// vkDestroyDescriptorSetLayout(m_device->vkDevice, vkDescriptorSetLayout, nullptr);
		m_cleaned = true;
	}
}
