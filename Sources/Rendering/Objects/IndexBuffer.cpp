#include "IndexBuffer.hpp"

#include "Model.hpp"
#include "Components/Buffer.hpp"
#include "Components/CommandPool.hpp"


IndexBuffer::IndexBuffer(const std::shared_ptr<Device>& device, const std::shared_ptr<Model>& model, const std::shared_ptr<CommandPool>& commandPool)
{
	VkDeviceSize bufferSize = sizeof(model->indices()[0]) * model->indices().size();

	Buffer indexStagingBuffer(device, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	void* data;
	vkMapMemory(device->vkDevice, indexStagingBuffer.vkDeviceMemory, 0, bufferSize, 0, &data);
	memcpy(data, model->indices().data(), (size_t)bufferSize);
	vkUnmapMemory(device->vkDevice, indexStagingBuffer.vkDeviceMemory);

	buffer = std::make_shared<Buffer>(device, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	CopyBuffer(indexStagingBuffer.vkBuffer, buffer->vkBuffer, bufferSize, commandPool);

	vkDestroyBuffer(device->vkDevice, indexStagingBuffer.vkBuffer, nullptr);
	vkFreeMemory(device->vkDevice, indexStagingBuffer.vkDeviceMemory, nullptr);
}

void IndexBuffer::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, const std::shared_ptr<CommandPool>& commandPool)
{
	VkCommandBuffer commandBuffer = commandPool->BeginSingleTimeCommands();
	VkBufferCopy copyRegion;
	copyRegion.srcOffset = 0; // Optional
	copyRegion.dstOffset = 0; // Optional
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
	commandPool->EndSingleTimeCommands(commandBuffer);
}
