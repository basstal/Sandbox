// #include "VertexBuffer.hpp"
// #include "GameCore/Model.hpp"
// #include "Rendering/Base/Device.hpp"
// #include "Rendering/Buffers/Buffer.hpp"
// #include "Rendering/Components/CommandResource.hpp"
//
//
// VertexBuffer::VertexBuffer(const std::shared_ptr<Device>& device, const void* inData, VkDeviceSize bufferSize, const std::shared_ptr<CommandResource>& commandResource)
// {
//     m_device = device;
//     Buffer vertexStagingBuffer(device, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
//
//     void* data;
//     vkMapMemory(device->vkDevice, vertexStagingBuffer.vkDeviceMemory, 0, bufferSize, 0, &data);
//     memcpy(data, inData, (size_t)bufferSize);
//     vkUnmapMemory(device->vkDevice, vertexStagingBuffer.vkDeviceMemory);
//
//     buffer = std::make_shared<Buffer>(device, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
//     CopyBuffer(vertexStagingBuffer.vkBuffer, buffer->vkBuffer, bufferSize, commandResource);
// }
//
// VertexBuffer::~VertexBuffer()
// {
//     Cleanup();
// }
//
// void VertexBuffer::Cleanup()
// {
//     if (!m_cleaned)
//     {
//         buffer->Cleanup();
//         m_cleaned = true;
//     }
// }
//
//
// void VertexBuffer::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, const std::shared_ptr<CommandResource>& commandResource)
// {
//     VkCommandBuffer commandBuffer = CommandResource::BeginSingleTimeGraphicsCommands(m_device);
//     VkBufferCopy copyRegion;
//     copyRegion.srcOffset = 0; // Optional
//     copyRegion.dstOffset = 0; // Optional
//     copyRegion.size = size;
//     vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
//     CommandResource::EndSingleTimeGraphicsCommands(m_device);
// }
