#include "IndexBuffer.hpp"

#include "Rendering/Model.hpp"
#include "Rendering/Buffers/Buffer.hpp"
#include "Rendering/Components/CommandResource.hpp"


IndexBuffer::IndexBuffer(const std::shared_ptr<Device>& device, const std::shared_ptr<Model>& model, const std::shared_ptr<CommandResource>& commandResource)
{
	VkDeviceSize bufferSize = sizeof(model->Indices()[0]) * model->Indices().size();

	Buffer indexStagingBuffer(device, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	void* data;
	vkMapMemory(device->vkDevice, indexStagingBuffer.vkDeviceMemory, 0, bufferSize, 0, &data);
	memcpy(data, model->Indices().data(), (size_t)bufferSize);
	vkUnmapMemory(device->vkDevice, indexStagingBuffer.vkDeviceMemory);

	buffer = std::make_shared<Buffer>(device, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	CopyBuffer(indexStagingBuffer.vkBuffer, buffer->vkBuffer, bufferSize, commandResource);
}
IndexBuffer::~IndexBuffer()
{
	Cleanup();
}
void IndexBuffer::Cleanup()
{
	if (!m_cleaned)
	{
		buffer->Cleanup();
		m_cleaned = true;
	}
}

void IndexBuffer::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, const std::shared_ptr<CommandResource>& commandResource)
{
	VkCommandBuffer commandBuffer = commandResource->BeginSingleTimeCommands();
	VkBufferCopy copyRegion;
	copyRegion.srcOffset = 0; // Optional
	copyRegion.dstOffset = 0; // Optional
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
	commandResource->EndSingleTimeCommands(commandBuffer);
}
