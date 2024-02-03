#pragma once
#include <memory>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "Model.hpp"
#include "Components/RenderPass.hpp"
#include "Base/Device.hpp"
#include "Base/Surface.hpp"
#include "Components/CommandResource.hpp"
#include "Components/Pipeline.hpp"
#include "Components\DescriptorResource.hpp"
#include "Components/Swapchain.hpp"

#include "Objects/IndexBuffer.hpp"
#include "Objects/RenderTexture.hpp"
#include "Objects/SyncObjects.hpp"
#include "Objects/UniformBuffers.hpp"
#include "Objects/VertexBuffer.hpp"

class ApplicationEditor;

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
	uint32_t m_currentFrame = 0;
	bool m_cleaned = false;

public:
	VkInstance vkInstance;
	std::shared_ptr<Surface> surface;
	std::shared_ptr<Device> device;
	std::shared_ptr<Swapchain> swapchain;
	std::shared_ptr<RenderPass> renderPass;
	std::shared_ptr<DescriptorResource> descriptorResource;
	std::shared_ptr<Pipeline> pipeline;
	std::shared_ptr<CommandResource> commandResource;

	// specific to usage
	std::shared_ptr<RenderTexture> renderTexture;
	std::shared_ptr<Model> model;
	std::shared_ptr<Image> image;
	std::shared_ptr<VertexBuffer> vertexBuffer;
	std::shared_ptr<IndexBuffer> indexBuffer;
	std::shared_ptr<UniformBuffers> uniformBuffers;
	std::shared_ptr<SyncObjects> syncObjects;

	glm::vec4 clearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	static std::unique_ptr<Application> Instance;
	Application();
	~Application();
	void Cleanup();
	void CreateSwapchain();
	static uint32_t FindMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);
	void DrawFrame(const std::unique_ptr<ApplicationEditor>& applicationEditor);
	void RecreateSwapchain(const std::unique_ptr<ApplicationEditor>& editor);
	void RecordCommandBuffer(VkCommandBuffer currentCommandBuffer, uint32_t imageIndex, const std::unique_ptr<ApplicationEditor>& applicationEditor);
};
