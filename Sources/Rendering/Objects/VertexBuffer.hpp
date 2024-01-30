#pragma once
#include "Rendering/Model.hpp"
#include "Rendering/Components/Buffer.hpp"
#include "Rendering/Components/CommandResource.hpp"

class VertexBuffer
{
private:
	void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, const std::shared_ptr<CommandResource>& commandResource);
	bool m_cleaned = false;

public:
	std::shared_ptr<Buffer> buffer;
	VertexBuffer(const std::shared_ptr<Device>& device, const std::shared_ptr<Model>& model, const std::shared_ptr<CommandResource>& commandResource);
	~VertexBuffer();
	void Cleanup();
};
