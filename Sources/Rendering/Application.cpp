#include "Application.hpp"

#include <filesystem>
#include <memory>
#include <stdexcept>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>

#include "Image.hpp"
#include "Model.hpp"
#include "..\Infrastructures\FileSystemBase.hpp"
#include "Components/RenderPass.hpp"
#include "Base/Device.hpp"
#include "Components/CommandResource.hpp"
#include "Editor/ApplicationEditor.hpp"
#include "Objects/RenderTexture.hpp"


// 定义和初始化静态成员变量
std::unique_ptr<Application> Application::Instance = nullptr;

std::vector<const char*> Application::GetRequiredExtensions()
{
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
	CheckExtensionsSupport(glfwExtensionCount, glfwExtensions);

	return extensions;
}

void Application::CheckExtensionsSupport(uint32_t glfwExtensionCount, const char** glfwExtensions)
{
	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
	for (uint32_t i = 0; i < glfwExtensionCount; i++)
	{
		bool isContained = false;
		for (const auto& extension : extensions)
		{
			if (strcmp(glfwExtensions[i], extension.extensionName) == 0)
			{
				isContained = true;
				break;
			}
		}
		if (!isContained)
		{
			throw std::runtime_error("glfwExtension is not contained in available extensions");
		}
	}
}

Application::Application(const std::shared_ptr<Settings>& settings)
{
	m_settings = settings;
	glfwInit();

	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = m_settings->ApplicationName.c_str();
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledLayerCount = 0;
	createInfo.pNext = nullptr;

	auto glfwExtensions = GetRequiredExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(glfwExtensions.size());
	createInfo.ppEnabledExtensionNames = glfwExtensions.data();

	if (vkCreateInstance(&createInfo, nullptr, &vkInstance) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create instance!");
	}
	surface = std::make_shared<Surface>(vkInstance, settings);
	device = std::make_shared<Device>(vkInstance, surface);
	CreateSwapchain();
	renderPass = std::make_shared<RenderPass>(device, swapchain, RenderPassType::GAME_RENDER_PASS);
	descriptorResource = std::make_shared<DescriptorResource>(device);
	pipeline = std::make_shared<Pipeline>(device, descriptorResource, renderPass);
	std::filesystem::path binariesDir = FileSystemBase::getBinariesDir();
	std::vector<char> vertexShader = FileSystemBase::readFile((binariesDir / "Shaders/Test_vert.spv").string());
	std::vector<char> fragmentShader = FileSystemBase::readFile((binariesDir / "Shaders/Test_frag.spv").string());
	pipeline->CreatePipeline(vertexShader, fragmentShader);
	swapchain->CreateFramebuffers(renderPass);
	commandResource = std::make_shared<CommandResource>(device);
	std::filesystem::path assetsDir = FileSystemBase::getAssetsDir();

	model = Model::loadModel((assetsDir / "Models/viking_room.obj").string().c_str());
	image = Image::loadImage((assetsDir / "Textures/viking_room.png").string().c_str());
	renderTexture = std::make_shared<RenderTexture>(device, image, commandResource);
	vertexBuffer = std::make_shared<VertexBuffer>(device, model, commandResource);
	indexBuffer = std::make_shared<IndexBuffer>(device, model, commandResource);
	uniformBuffers = std::make_shared<UniformBuffers>(device);
	descriptorResource->CreateDescriptorPool();
	descriptorResource->CreateDescriptorSets(uniformBuffers, renderTexture);
	commandResource->CreateCommandBuffers();
	syncObjects = std::make_shared<SyncObjects>(device);
}

Application::~Application()
{
	Cleanup();
}

void Application::Cleanup()
{
	if (m_cleaned)
	{
		return;
	}
	swapchain->Cleanup();

	renderTexture->Cleanup();
	indexBuffer->Cleanup();
	vertexBuffer->Cleanup();
	uniformBuffers->Cleanup();
	pipeline->Cleanup();
	descriptorResource->Cleanup();
	renderPass->Cleanup();
	syncObjects->Cleanup();
	commandResource->Cleanup();
	device->Cleanup();
	surface->Cleanup();
	vkDestroyInstance(vkInstance, nullptr);
	glfwTerminate();
	if (m_settings != nullptr)
	{
		m_settings->Save();
	}
	m_cleaned = true;
}

void Application::CreateSwapchain()
{
	swapchain = std::make_shared<Swapchain>(surface, device);
}

uint32_t Application::FindMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if (typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
		{
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}

void Application::DrawFrame(const std::unique_ptr<ApplicationEditor>& applicationEditor)
{
	vkWaitForFences(device->vkDevice, 1, &syncObjects->inFlightFences[m_currentFrame], VK_TRUE, UINT64_MAX);
	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(device->vkDevice, swapchain->vkSwapchain, UINT64_MAX, syncObjects->imageAvailableSemaphores[m_currentFrame], VK_NULL_HANDLE, &imageIndex);
	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		RecreateSwapchain(applicationEditor);
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("failed to acquire swap chain image!");
	}

	// Only reset the fence if we are submitting work
	VkCommandBuffer currentCommandBuffer = commandResource->vkCommandBuffers[m_currentFrame];
	vkResetFences(device->vkDevice, 1, &syncObjects->inFlightFences[m_currentFrame]);
	vkResetCommandBuffer(currentCommandBuffer, 0);
	RecordCommandBuffer(currentCommandBuffer, imageIndex, applicationEditor);
	uniformBuffers->UpdateUniformBuffer(m_currentFrame, swapchain->vkExtent2D);
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = {syncObjects->imageAvailableSemaphores[m_currentFrame]};
	VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &currentCommandBuffer;
	VkSemaphore signalSemaphoresGame[] = {syncObjects->gameRenderFinishedSemaphores[m_currentFrame]};
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphoresGame;
	if (vkQueueSubmit(device->graphicsQueue, 1, &submitInfo, syncObjects->inFlightFences[m_currentFrame]) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to submit draw command buffer!");
	}
	vkWaitForFences(device->vkDevice, 1, &syncObjects->inFlightFences[m_currentFrame], VK_TRUE, UINT64_MAX);
	vkResetFences(device->vkDevice, 1, &syncObjects->inFlightFences[m_currentFrame]);

	// VkImageMemoryBarrier imageMemoryBarrier = {};
	// imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	// imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // 当前布局
	// imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; // 目标布局
	// imageMemoryBarrier.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT; // 在转换之前必须完成的操作
	// imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT; // 在转换之后必须完成的操作
	// imageMemoryBarrier.image = swapchain->vkImages[m_currentFrame]; // 需要转换布局的图像
	// imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT; // 指定影响的图像方面
	// imageMemoryBarrier.subresourceRange.baseMipLevel = 0; // 起始mip级别
	// imageMemoryBarrier.subresourceRange.levelCount = 1; // mip级别的数量
	// imageMemoryBarrier.subresourceRange.baseArrayLayer = 0; // 起始数组层
	// imageMemoryBarrier.subresourceRange.layerCount = 1; // 数组层的数量
	//
	// vkCmdPipelineBarrier(
	// 	currentCommandBuffer,
	// 	VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, // 生产者阶段
	// 	VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, // 消费者阶段
	// 	0, // 标志
	// 	0, nullptr,
	// 	0, nullptr,
	// 	1, &imageMemoryBarrier); // 图像内存屏障


	applicationEditor->DrawFrame(*this, currentCommandBuffer, m_currentFrame, syncObjects, imageIndex);

	VkSemaphore presentWaitSemaphores[] = {syncObjects->renderFinishedSemaphores[m_currentFrame]};
	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = presentWaitSemaphores;

	VkSwapchainKHR swapChains[] = {swapchain->vkSwapchain};
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr; // Optional
	result = vkQueuePresentKHR(device->presentQueue, &presentInfo);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || surface->GetFrameBufferResized())
	{
		surface->SetFrameBufferResized(false);
		RecreateSwapchain(applicationEditor);
	}
	else if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to present swap chain image!");
	}
	m_currentFrame = (m_currentFrame + 1) % device->MAX_FRAMES_IN_FLIGHT;
}

void Application::RecreateSwapchain(const std::unique_ptr<ApplicationEditor>& editor)
{
	int width = 0, height = 0;
	glfwGetFramebufferSize(surface->glfwWindow, &width, &height);
	while (width == 0 || height == 0)
	{
		glfwGetFramebufferSize(surface->glfwWindow, &width, &height);
		glfwWaitEvents();
	}
	vkDeviceWaitIdle(device->vkDevice);

	swapchain->Cleanup();
	editor->CleanupWhenRecreateSwapchain();
	swapchain->CreateSwapchain(surface, device);
	swapchain->CreateFramebuffers(renderPass);
	editor->CreateFramebuffer(Instance);
}

void Application::RecordCommandBuffer(VkCommandBuffer currentCommandBuffer, uint32_t imageIndex, const std::unique_ptr<ApplicationEditor>& applicationEditor)
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0; // Optional
	beginInfo.pInheritanceInfo = nullptr; // Optional

	if (vkBeginCommandBuffer(currentCommandBuffer, &beginInfo) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to begin recording command buffer!");
	}

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = renderPass->vkRenderPass;
	renderPassInfo.framebuffer = swapchain->vkFramebuffers[imageIndex];
	renderPassInfo.renderArea.offset = {0, 0};
	renderPassInfo.renderArea.extent = swapchain->vkExtent2D;
	std::array<VkClearValue, 2> clearValues{};
	clearValues[0].color = {{clearColor.r, clearColor.g, clearColor.b, clearColor.a}};
	clearValues[1].depthStencil = {1.0f, 0};

	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();
	vkCmdBeginRenderPass(currentCommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdBindPipeline(currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GraphicsPipeline());

	VkViewport viewport;
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(swapchain->vkExtent2D.width);
	viewport.height = static_cast<float>(swapchain->vkExtent2D.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(currentCommandBuffer, 0, 1, &viewport);

	VkRect2D scissor;
	scissor.offset = {0, 0};
	scissor.extent = swapchain->vkExtent2D;
	vkCmdSetScissor(currentCommandBuffer, 0, 1, &scissor);

	VkBuffer vertexBuffers[] = {vertexBuffer->buffer->vkBuffer};
	VkDeviceSize offsets[] = {0};
	vkCmdBindVertexBuffers(currentCommandBuffer, 0, 1, vertexBuffers, offsets);

	vkCmdBindIndexBuffer(currentCommandBuffer, indexBuffer->buffer->vkBuffer, 0, VK_INDEX_TYPE_UINT32);
	vkCmdBindDescriptorSets(currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->vkPipelineLayout, 0, 1, &descriptorResource->vkDescriptorSets[m_currentFrame], 0, nullptr);
	vkCmdDrawIndexed(currentCommandBuffer, static_cast<uint32_t>(model->indices().size()), 1, 0, 0, 0);

	vkCmdEndRenderPass(currentCommandBuffer);

	if (vkEndCommandBuffer(currentCommandBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to record command buffer!");
	}
}
