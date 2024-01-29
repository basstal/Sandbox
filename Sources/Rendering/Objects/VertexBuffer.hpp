#pragma once
#include "Model.hpp"
#include "Components/Buffer.hpp"
#include "Components/CommandPool.hpp"

class VertexBuffer
{
	void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, const std::shared_ptr<CommandPool>& commandPool);

public:
	std::shared_ptr<Buffer> buffer;
	VertexBuffer(const std::shared_ptr<Device>& device, const std::shared_ptr<Model>& model, const std::shared_ptr<CommandPool>& commandPool);
};
