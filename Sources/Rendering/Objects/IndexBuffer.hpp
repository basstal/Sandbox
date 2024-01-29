#pragma once
#include "Model.hpp"
#include "Components/CommandPool.hpp"
#include "Components/Buffer.hpp"

class IndexBuffer
{
	void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, const std::shared_ptr<CommandPool>& commandPool);

public:
	std::shared_ptr<Buffer> buffer;
	IndexBuffer(const std::shared_ptr<Device>& device, const std::shared_ptr<Model>& model, const std::shared_ptr<CommandPool>& commandPool);
};
