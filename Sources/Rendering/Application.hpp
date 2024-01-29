﻿#pragma once
#include <memory>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "Model.hpp"
#include "Components/RenderPass.hpp"
#include "Base/Device.hpp"
#include "Base/Surface.hpp"
#include "Components/CommandPool.hpp"
#include "Components/Pipeline.hpp"
#include "Components\DescriptorSet.hpp"
#include "Components/Swapchain.hpp"
#include "Objects/IndexBuffer.hpp"
#include "Objects/RenderTexture.hpp"
#include "Objects/UniformBuffers.hpp"
#include "Objects/VertexBuffer.hpp"

class Application
{
private:
	/**
	 * \brief 获得应用程序所需的扩展的名称列表
	 * \return 
	 */
	std::vector<const char*> GetRequiredExtensions();
	/**
	 * \brief 检查传入的 glfwExtensions 是否都是被支持的扩展
	 * \param glfwExtensionCount 
	 * \param glfwExtensions 
	 */
	void CheckExtensionsSupport(uint32_t glfwExtensionCount, const char** glfwExtensions);

public:
	VkInstance vkInstance;
	std::shared_ptr<Surface> surface;
	std::shared_ptr<Device> device;
	std::shared_ptr<Swapchain> swapchain;
	std::shared_ptr<RenderPass> renderPass;
	std::shared_ptr<DescriptorSet> descriptorSet;
	std::shared_ptr<Pipeline> pipeline;
	std::shared_ptr<CommandPool> commandPool;

	// specific to usage
	std::shared_ptr<RenderTexture> renderTexture;
	std::shared_ptr<Model> model;
	std::shared_ptr<Image> image;
	std::shared_ptr<VertexBuffer> vertexBuffer;
	std::shared_ptr<IndexBuffer> indexBuffer;
	std::shared_ptr<UniformBuffers> uniformBuffers;

	Application();
	~Application();
	void CreateSwapchain();
	static uint32_t FindMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);
};
