#pragma once
#include "Rendering/Base/Device.hpp"
#include "Rendering/Components/Buffer.hpp"

class UniformBuffers
{
private:
	bool m_cleanup = false;

public:
	std::vector<std::shared_ptr<Buffer>> uniformBuffers;
	std::vector<void*> uniformBuffersMapped;
	UniformBuffers(const std::shared_ptr<Device>& device);
	~UniformBuffers();
	void UpdateUniformBuffer(uint32_t currentImage, VkExtent2D extent2D);
	void Cleanup();
};
