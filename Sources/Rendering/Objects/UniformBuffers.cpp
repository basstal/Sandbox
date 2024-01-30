#include "UniformBuffers.hpp"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "Rendering/Camera.hpp"
#include "Rendering/Base/Device.hpp"


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

UniformBuffers::~UniformBuffers()
{
	Cleanup();
}

void UniformBuffers::UpdateUniformBuffer(uint32_t currentImage, VkExtent2D extent2D)
{
	UniformBufferObject ubo;
	//static auto startTime = std::chrono::high_resolution_clock::now();

	//auto currentTime = std::chrono::high_resolution_clock::now();
	//float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
	//ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.model = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.proj = glm::perspective(glm::radians(45.0f), (float)extent2D.width / (float)extent2D.height, 0.1f, 10.0f);
	ubo.proj[1][1] *= -1;
	// NOTE: Using a UBO this way is not the most efficient way to pass frequently changing values to the shader. A more efficient way to pass a small buffer of data to shaders are push constants.
	memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
}

void UniformBuffers::Cleanup()
{
	if (!m_cleanup)
	{
		for (size_t i = 0; i < uniformBuffers.size(); i++)
		{
			uniformBuffers[i]->Cleanup();
		}
		for (size_t i = 0; i < uniformBuffersMapped.size(); i++)
		{
			uniformBuffersMapped[i] = nullptr;
		}
		m_cleanup = true;
	}
}
