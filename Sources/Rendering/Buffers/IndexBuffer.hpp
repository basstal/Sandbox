#pragma once
#include "GameCore/Model.hpp"
#include "Rendering/Components/CommandResource.hpp"
#include "Rendering/Buffers/Buffer.hpp"

class IndexBuffer
{
	void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, const std::shared_ptr<CommandResource>& commandResource);
	bool m_cleaned = false;

public:
	std::shared_ptr<Buffer> buffer;
	IndexBuffer(const std::shared_ptr<Device>& device, const void* inData, VkDeviceSize bufferSize, const std::shared_ptr<CommandResource>& commandResource);
	~IndexBuffer();
	void Cleanup();
};
