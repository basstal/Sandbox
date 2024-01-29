#include "UniformBuffers.hpp"

#include "Camera.hpp"
#include "Base/Device.hpp"


UniformBuffers::UniformBuffers(const std::shared_ptr<Device>& device)
{
	VkDeviceSize bufferSize = sizeof(UniformBufferObject);

	uniformBuffers.resize(device->MAX_FRAMES_IN_FLIGHT);
	uniformBuffersMapped.resize(device->MAX_FRAMES_IN_FLIGHT);

	for (size_t i = 0; i < device->MAX_FRAMES_IN_FLIGHT; i++)
	{
		uniformBuffers[i] = std::make_shared<Buffer>(device, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		vkMapMemory(device->vkDevice, uniformBuffers[i]->vkDeviceMemory, 0, bufferSize, 0, &uniformBuffersMapped[i]);
	}
}
