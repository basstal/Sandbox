#include "UniformBuffers.hpp"

#include "Rendering/Camera.hpp"
#include "Rendering/Model.hpp"
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

UniformBufferObject UniformBuffers::UpdateUniformBuffer(uint32_t currentImage, const std::shared_ptr<Camera>& camera, const std::shared_ptr<Model>& model,
                                                        const glm::mat4& projection)
{
	UniformBufferObject ubo;
	ubo.model = model->transform->GetModelMatrix();
	ubo.view = camera->GetViewMatrix();
	ubo.proj = projection;
	ubo.proj[1][1] *= -1;
	// NOTE: Using a UBO this way is not the most efficient way to pass frequently changing values to the shader. A more efficient way to pass a small buffer of data to shaders are push constants.
	memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
	return ubo;
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
