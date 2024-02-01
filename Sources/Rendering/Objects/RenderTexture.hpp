#pragma once
#include <memory>
#include <vulkan/vulkan_core.h>

#include "Rendering/Image.hpp"
#include "Rendering/Base/Device.hpp"
#include "Rendering/Components/CommandResource.hpp"


class RenderTexture
{
private:
	void CopyFromBuffer(VkBuffer buffer, uint32_t width, uint32_t height);
	void GenerateMipmaps(VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);
	void CreateTextureSampler();
	void TransitionImageLayout(VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);
	std::shared_ptr<CommandResource> m_commandPool;
	std::shared_ptr<Device> m_device;

	bool m_cleaned = false;

public:
	VkImage vkImage;
	VkDeviceMemory vkDeviceMemory;
	VkImageView vkImageView;
	VkSampler vkSampler;
	RenderTexture(const std::shared_ptr<Device>& device, const std::shared_ptr<Image>& image, const std::shared_ptr<CommandResource>& commandResource);
	~RenderTexture();
	void Cleanup();
};
