﻿#include "UniformBuffers.hpp"

#include "Rendering/Camera.hpp"
#include "Rendering/Light.hpp"
#include "Rendering/Model.hpp"
#include "Rendering/Base/Device.hpp"


UniformBuffers::UniformBuffers(const std::shared_ptr<Device>& device)
{
	mvpObjectBuffers.resize(device->MAX_FRAMES_IN_FLIGHT);
	mvpObjectBuffersMapped.resize(device->MAX_FRAMES_IN_FLIGHT);
	for (size_t i = 0; i < device->MAX_FRAMES_IN_FLIGHT; i++)
	{
		mvpObjectBuffers[i] = std::make_shared<Buffer>(device, sizeof(MVPObject), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		vkMapMemory(device->vkDevice, mvpObjectBuffers[i]->vkDeviceMemory, 0, sizeof(MVPObject), 0, &mvpObjectBuffersMapped[i]);
	}

	// pbrMaterialBuffers.resize(device->MAX_FRAMES_IN_FLIGHT);
	// pbrMaterialBuffersMapped.resize(device->MAX_FRAMES_IN_FLIGHT);
	// for (size_t i = 0; i < device->MAX_FRAMES_IN_FLIGHT; i++)
	// {
	// 	pbrMaterialBuffers[i] = std::make_shared<Buffer>(device, sizeof(PBRMaterial), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	// 	vkMapMemory(device->vkDevice, pbrMaterialBuffers[i]->vkDeviceMemory, 0, sizeof(PBRMaterial), 0, &pbrMaterialBuffersMapped[i]);
	// }

	pbrLightBuffers.resize(device->MAX_FRAMES_IN_FLIGHT);
	pbrLightBuffersMapped.resize(device->MAX_FRAMES_IN_FLIGHT);
	for (size_t i = 0; i < device->MAX_FRAMES_IN_FLIGHT; i++)
	{
		pbrLightBuffers[i] = std::make_shared<Buffer>(device, sizeof(Light), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		vkMapMemory(device->vkDevice, pbrLightBuffers[i]->vkDeviceMemory, 0, sizeof(Light), 0, &pbrLightBuffersMapped[i]);
	}
}

UniformBuffers::~UniformBuffers()
{
	Cleanup();
}

MVPObject UniformBuffers::UpdateMVP(uint32_t currentImage, const std::shared_ptr<Camera>& camera, const glm::mat4& model,
                                    const glm::mat4& projection)
{
	MVPObject ubo;
	ubo.model = model;
	ubo.view = camera->GetViewMatrix();
	ubo.proj = projection;
	ubo.proj[1][1] *= -1;
	// NOTE: Using a UBO this way is not the most efficient way to pass frequently changing values to the shader. A more efficient way to pass a small buffer of data to shaders are push constants.
	memcpy(mvpObjectBuffersMapped[currentImage], &ubo, sizeof(ubo));
	return ubo;
}

MVPObject UniformBuffers::UpdateMVP(uint32_t currentImage, const glm::mat4& view, const glm::mat4& model, const glm::mat4& projection)
{
	MVPObject ubo;
	ubo.model = model;
	ubo.view = view;
	ubo.proj = projection;
	ubo.proj[1][1] *= -1;
	// NOTE: Using a UBO this way is not the most efficient way to pass frequently changing values to the shader. A more efficient way to pass a small buffer of data to shaders are push constants.
	memcpy(mvpObjectBuffersMapped[currentImage], &ubo, sizeof(ubo));
	return ubo;
}

// void UniformBuffers::UpdatePBRMaterial(uint32_t currentImage)
// {
// 	PBRMaterial ubo;
// 	ubo.albedo = glm::vec3(0.5f, 0.0f, 0.0f);
// 	ubo.metallic = 0.0f;
// 	ubo.roughness = 0.0f;
// 	ubo.ao = 1.0f;
// 	memcpy(pbrMaterialBuffersMapped[currentImage], &ubo, sizeof(ubo));
// }

void UniformBuffers::UpdatePBRLight(uint32_t currentImage, const glm::vec3& position, const glm::vec3& color)
{
	Light light(position, color);
	// light.intensity = 1.0f;
	memcpy(pbrLightBuffersMapped[currentImage], &light, sizeof(light));
}

void UniformBuffers::Cleanup()
{
	if (!m_cleanup)
	{
		for (size_t i = 0; i < mvpObjectBuffers.size(); i++)
		{
			mvpObjectBuffers[i]->Cleanup();
		}
		for (size_t i = 0; i < mvpObjectBuffersMapped.size(); i++)
		{
			mvpObjectBuffersMapped[i] = nullptr;
		}
		// for (size_t i = 0; i < pbrMaterialBuffers.size(); i++)
		// {
		// 	pbrMaterialBuffers[i]->Cleanup();
		// }
		// for (size_t i = 0; i < pbrMaterialBuffersMapped.size(); i++)
		// {
		// 	pbrMaterialBuffersMapped[i] = nullptr;
		// }
		for (size_t i = 0; i < pbrLightBuffers.size(); i++)
		{
			pbrLightBuffers[i]->Cleanup();
		}
		for (size_t i = 0; i < pbrLightBuffersMapped.size(); i++)
		{
			pbrLightBuffersMapped[i] = nullptr;
		}
		m_cleanup = true;
	}
}
