#include "SyncObjects.hpp"

#include <memory>
#include <stdexcept>

#include "Rendering/Base/Device.hpp"

SyncObjects::SyncObjects(const std::shared_ptr<Device>& device)
{
	m_device = device;
	imageAvailableSemaphores.resize(device->MAX_FRAMES_IN_FLIGHT);
	renderFinishedSemaphores.resize(device->MAX_FRAMES_IN_FLIGHT);
	gameRenderFinishedSemaphores.resize(device->MAX_FRAMES_IN_FLIGHT);
	inFlightFences.resize(device->MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
	for (size_t i = 0; i < device->MAX_FRAMES_IN_FLIGHT; i++)
	{
		if (vkCreateSemaphore(device->vkDevice, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(device->vkDevice, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(device->vkDevice, &semaphoreInfo, nullptr, &gameRenderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(device->vkDevice, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create synchronization objects for a frame!");
		}
	}
}

SyncObjects::~SyncObjects()
{
	Cleanup();
}
void SyncObjects::Cleanup()
{
	if (m_cleanup)
	{
		return;
	}
	for (uint32_t i = 0; i < renderFinishedSemaphores.size(); i++)
	{
		vkDestroySemaphore(m_device->vkDevice, renderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(m_device->vkDevice, imageAvailableSemaphores[i], nullptr);
		vkDestroySemaphore(m_device->vkDevice, gameRenderFinishedSemaphores[i], nullptr);
		vkDestroyFence(m_device->vkDevice, inFlightFences[i], nullptr);
	}
	m_cleanup = true;
}
