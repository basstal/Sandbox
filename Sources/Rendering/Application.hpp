﻿#pragma once
#include <memory>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "Camera.hpp"
#include "GameCore/Model.hpp"
#include "Settings.hpp"
#include "Components/RenderPass.hpp"
#include "Base/Device.hpp"
#include "Base/Surface.hpp"
#include "Components/CommandResource.hpp"
#include "Components/Pipeline.hpp"
#include "Components\DescriptorResource.hpp"
#include "Components/Swapchain.hpp"
#include "GameCore/GameObject.hpp"
#include "Infrastructures/Timer.hpp"

#include "Rendering/Buffers/IndexBuffer.hpp"
#include "Objects/RenderTexture.hpp"
#include "Objects/SyncObjects.hpp"
#include "Rendering/Buffers/UniformBuffers.hpp"
#include "Rendering/Buffers/VertexBuffer.hpp"

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
	Application();
	~Application();
	VkInstance vkInstance;
	std::shared_ptr<Surface> surface;
	std::shared_ptr<Device> device;
	std::shared_ptr<Swapchain> swapchain;
	std::shared_ptr<RenderPass> renderPass;
	// std::shared_ptr<DescriptorResource> descriptorResource;
	std::shared_ptr<Pipeline> mainPipeline;
	std::shared_ptr<CommandResource> commandResource;

	// specific to usage
	// std::shared_ptr<Shader> pbrShader;
	std::shared_ptr<RenderTexture> renderTexture;
	std::shared_ptr<GameObject> modelGameObject;
	std::shared_ptr<Image> image;
	// std::vector<char> vertexShader;
	// std::vector<char> fragmentShader;
	std::shared_ptr<VertexBuffer> vertexBuffer;
	std::shared_ptr<IndexBuffer> indexBuffer;
	std::shared_ptr<UniformBuffers> uniformBuffers;
	std::shared_ptr<SyncObjects> syncObjects;

	std::shared_ptr<Material> material;

	std::shared_ptr<Settings> settings;
	std::shared_ptr<Camera> editorCamera;
	std::shared_ptr<Timer> timer;

	glm::vec4 clearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	MVPObject debugUBO;
	glm::mat4 projection;
	float deltaTime;

	void Cleanup();
	void Initialize();
	void LoadAssets();
	void CreateSwapchain();
	void DrawFrame(const std::shared_ptr<ApplicationEditor>& applicationEditor);
	void RecreateSwapchain(const std::shared_ptr<ApplicationEditor>& editor);
	void RecordCommandBuffer(VkCommandBuffer currentCommandBuffer, uint32_t imageIndex, const std::shared_ptr<ApplicationEditor>& applicationEditor);
	VkDescriptorSet CreateDescriptorSet(const VkDescriptorSetLayout& descriptorSetLayout, const std::shared_ptr<UniformBuffers>& inUniformBuffers,
	                                    const std::shared_ptr<DescriptorResource>& inDescriptorResource, const std::shared_ptr<RenderTexture>& inRenderTexture);
	void Draw(const glm::vec3& position, const std::shared_ptr<RenderTexture>& inRenderTexture, const VkCommandBuffer& commandBuffer);
};
