#pragma once
#include "Model.hpp"
#include "Components/CommandResource.hpp"
#include "Components/Buffer.hpp"

class IndexBuffer
{
	void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, const std::shared_ptr<CommandResource>& commandResource);
	bool m_cleaned = false;

public:
	std::shared_ptr<Buffer> buffer;
	IndexBuffer(const std::shared_ptr<Device>& device, const std::shared_ptr<Model>& model, const std::shared_ptr<CommandResource>& commandResource);
	~IndexBuffer();
	void Cleanup();
};
