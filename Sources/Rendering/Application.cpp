#include "Application.hpp"

#include <filesystem>
#include <memory>
#include <stdexcept>
#include <GLFW/glfw3.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <vulkan/vulkan_core.h>

#include "Image.hpp"
#include "Model.hpp"
#include "Infrastructures/FileSystem/FileSystemBase.hpp"
#include "Components/RenderPass.hpp"
#include "Base/Device.hpp"
#include "Components/CommandResource.hpp"
#include "Editor/ApplicationEditor.hpp"
#include "Infrastructures/DataBinding.hpp"
#include "Infrastructures/FileSystem/File.hpp"
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

Application::Application(const std::shared_ptr<Settings>& inSettings)
{
	settings = inSettings;
	glfwInit();

	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = settings->ApplicationName.c_str();
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
	Shader::Uninitialize();
	if (material != nullptr)
	{
		material->Cleanup();
	}
	if (swapchain != nullptr)
	{
		swapchain->Cleanup();
	}
	if (renderTexture != nullptr)
	{
		renderTexture->Cleanup();
	}
	if (indexBuffer != nullptr)
	{
		indexBuffer->Cleanup();
	}
	if (vertexBuffer != nullptr)
	{
		vertexBuffer->Cleanup();
	}
	if (uniformBuffers != nullptr)
	{
		uniformBuffers->Cleanup();
	}
	if (mainPipeline != nullptr)
	{
		mainPipeline->Cleanup();
	}
	// if (descriptorResource != nullptr)
	// {
	// 	descriptorResource->Cleanup();
	// }
	if (renderPass != nullptr)
	{
		renderPass->Cleanup();
	}
	if (syncObjects != nullptr)
	{
		syncObjects->Cleanup();
	}
	if (commandResource != nullptr)
	{
		commandResource->Cleanup();
	}

	if (device != nullptr)
	{
		device->Cleanup();
	}
	if (surface != nullptr)
	{
		surface->Cleanup();
	}
	vkDestroyInstance(vkInstance, nullptr);
	glfwTerminate();


	if (settings != nullptr)
	{
		settings->Save();
	}
	m_cleaned = true;
}


void Application::Initialize()
{
	LoadAssets();
	CreateSwapchain();
	auto subpass = std::make_shared<Subpass>(device, true);
	subpass->BeginSubpassAttachments();
	subpass->AddColorAttachment("game", Swapchain::COLOR_FORMAT);
	subpass->AssignDepthAttachment("game_depth");
	subpass->EndSubpassAttachments();
	renderPass = std::make_shared<RenderPass>(device, subpass);
	// descriptorResource = std::make_shared<DescriptorResource>(device);
	// pipeline = std::make_shared<Pipeline>(device, descriptorResource, renderPass);

	Shader::Initialize();
	auto pbrShader = std::make_shared<Shader>(device);
	std::filesystem::path sourceDir = FileSystemBase::getSourceDir();
	pbrShader->LoadShaderForStage(std::make_shared<File>((sourceDir / "Shaders/PBR.vert").string()), "", VK_SHADER_STAGE_VERTEX_BIT);
	pbrShader->LoadShaderForStage(std::make_shared<File>((sourceDir / "Shaders/PBR.frag").string()), "", VK_SHADER_STAGE_FRAGMENT_BIT);
	mainPipeline = std::make_shared<Pipeline>(device, pbrShader, renderPass, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_POLYGON_MODE_FILL);
	// 将资源缓冲区绑定到 pipeline
	uniformBuffers = std::make_shared<UniformBuffers>(device);
	uniformBuffers->UpdateWriteDescriptorSet(mainPipeline->descriptorResource);


	// mainPipeline->CreateFillModeNonSolidPipeline();
	swapchain->CreateFramebuffers(renderPass);
	commandResource = std::make_shared<CommandResource>(device);

	// renderTexture = std::make_shared<RenderTexture>(device, commandResource, false, false, swapchain->vkExtent2D.width, swapchain->vkExtent2D.height, 1);
	vertexBuffer = std::make_shared<VertexBuffer>(device, modelGameObject->model, commandResource);
	indexBuffer = std::make_shared<IndexBuffer>(device, modelGameObject->model, commandResource);
	// descriptorResource->CreateDescriptorPool();

	std::filesystem::path assetsDir = FileSystemBase::getAssetsDir();
	auto albedo = Image::LoadImage((assetsDir / "Textures/pbr/rusted_iron/albedo.png").string().c_str());
	auto metallic = Image::LoadImage((assetsDir / "Textures/pbr/rusted_iron/metallic.png").string().c_str());
	auto roughness = Image::LoadImage((assetsDir / "Textures/pbr/rusted_iron/roughness.png").string().c_str());
	auto ao = Image::LoadImage((assetsDir / "Textures/pbr/rusted_iron/ao.png").string().c_str());
	auto irradiance = Image::LoadHdrImage((assetsDir / "Textures/hdr/newport_loft.hdr").string().c_str());

	material = std::make_shared<Material>(device, albedo, metallic, roughness, ao, irradiance, commandResource, mainPipeline, renderPass, mainPipeline->descriptorResource);
	material->TransitionImageLayout();
	commandResource->CreateCommandBuffers();
	syncObjects = std::make_shared<SyncObjects>(device);
	editorCamera = std::make_shared<Camera>(settings->EditorCameraPos, DEFAULT_UP, settings->EditorCameraRotationX, settings->EditorCameraRotationZ);
	DataBinding::Create("Rendering/EditorCamera", editorCamera);
	timer = std::make_shared<Timer>();
	projection = glm::perspective(glm::radians(45.0f), (float)swapchain->vkExtent2D.width / (float)swapchain->vkExtent2D.height, 0.1f, 100.f);
	// material->irradianceMap->RenderCube(commandResource, uniformBuffers, mainPipeline->descriptorResource);
}

void Application::LoadAssets()
{
	// std::filesystem::path binariesDir = FileSystemBase::getBinariesDir();
	// vertexShader = FileSystemBase::readFile((binariesDir / "Shaders/PBR_vert.spv").string());
	// fragmentShader = FileSystemBase::readFile((binariesDir / "Shaders/PBR_frag.spv").string());
	std::filesystem::path assetsDir = FileSystemBase::getAssetsDir();
	modelGameObject = std::make_shared<GameObject>();
	modelGameObject->model = Model::LoadModel((assetsDir / "Models/viking_room.obj").string().c_str());
	image = Image::LoadImage((assetsDir / "Textures/viking_room.png").string().c_str());
}

void Application::CreateSwapchain()
{
	swapchain = std::make_shared<Swapchain>(surface, device);
}


void Application::DrawFrame(const std::shared_ptr<ApplicationEditor>& applicationEditor)
{
	deltaTime = timer->GetDeltaTime();
	vkWaitForFences(device->vkDevice, 1, &syncObjects->inFlightFences[m_currentFrame], VK_TRUE, UINT64_MAX);
	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(device->vkDevice, swapchain->vkSwapchain, UINT64_MAX, syncObjects->imageAvailableSemaphores[m_currentFrame], VK_NULL_HANDLE, &imageIndex);
	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		RecreateSwapchain(applicationEditor);
		return;
	}
	if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("failed to acquire swap chain image!");
	}

	// Only reset the fence if we are submitting work
	VkCommandBuffer currentCommandBuffer = commandResource->vkCommandBuffers[m_currentFrame];
	vkResetFences(device->vkDevice, 1, &syncObjects->inFlightFences[m_currentFrame]);
	vkResetCommandBuffer(currentCommandBuffer, 0);
	RecordCommandBuffer(currentCommandBuffer, imageIndex, applicationEditor);

	auto modelMatrix = modelGameObject->transform->GetModelMatrix();
	debugUBO = uniformBuffers->UpdateMVP(m_currentFrame, editorCamera, modelMatrix, projection);
	uniformBuffers->UpdatePBRLight(m_currentFrame, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f));


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
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || surface->framebufferResized)
	{
		surface->framebufferResized = false;
		RecreateSwapchain(applicationEditor);
	}
	else if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to present swap chain image!");
	}
	m_currentFrame = (m_currentFrame + 1) % device->MAX_FRAMES_IN_FLIGHT;
}

void Application::RecreateSwapchain(const std::shared_ptr<ApplicationEditor>& editor)
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

void Application::RecordCommandBuffer(VkCommandBuffer currentCommandBuffer, uint32_t imageIndex, const std::shared_ptr<ApplicationEditor>& applicationEditor)
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0; // Optional
	beginInfo.pInheritanceInfo = nullptr; // Optional

	if (vkBeginCommandBuffer(currentCommandBuffer, &beginInfo) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to begin recording command buffer!");
	}

	VkClearColorValue clearColorValue = {{clearColor.r, clearColor.g, clearColor.b, clearColor.a}};
	VkClearDepthStencilValue clearDepthStencilValue = {1.0f, 0};
	renderPass->BeginRenderPass(currentCommandBuffer, swapchain->vkFramebuffers[imageIndex], swapchain->vkExtent2D, clearColorValue, clearDepthStencilValue);

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

	applicationEditor->grid->uniformBuffer->UpdateMVP(m_currentFrame, editorCamera, glm::mat4(1.0f), projection);
	applicationEditor->grid->Draw(device, currentCommandBuffer, mainPipeline->descriptorResource, m_currentFrame);
	vkCmdBindPipeline(currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mainPipeline->GraphicsPipeline());
	VkBuffer vertexBuffers[] = {vertexBuffer->buffer->vkBuffer};
	VkDeviceSize offsets[] = {0};
	vkCmdPushConstants(currentCommandBuffer, mainPipeline->vkPipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(glm::vec3), &editorCamera->position);
	vkCmdBindVertexBuffers(currentCommandBuffer, 0, 1, vertexBuffers, offsets);
	vkCmdBindIndexBuffer(currentCommandBuffer, indexBuffer->buffer->vkBuffer, 0, VK_INDEX_TYPE_UINT32);
	vkCmdBindDescriptorSets(currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mainPipeline->vkPipelineLayout, 0, 1, &mainPipeline->descriptorResource->vkDescriptorSets[m_currentFrame], 0,
	                        nullptr);
	vkCmdDrawIndexed(currentCommandBuffer, static_cast<uint32_t>(modelGameObject->model->Indices().size()), 1, 0, 0, 0);

	// VkBufferMemoryBarrier bufferMemoryBarrier = {};
	// bufferMemoryBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
	// bufferMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT; // 如果之前的操作是CPU写入或其他写入操作
	// bufferMemoryBarrier.dstAccessMask = VK_ACCESS_UNIFORM_READ_BIT; // 接下来需要进行Uniform读取
	// bufferMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	// bufferMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	// bufferMemoryBarrier.buffer = uniformBuffers->mvpObjectBuffers[m_currentFrame]->vkBuffer;
	// bufferMemoryBarrier.offset = 0;
	// bufferMemoryBarrier.size = VK_WHOLE_SIZE;
	//
	// vkCmdPipelineBarrier(
	// 	currentCommandBuffer,
	// 	VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, // 从管道的顶部开始同步
	// 	VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, // 目标是顶点着色器阶段
	// 	0, // Flags
	// 	0, nullptr, // 内存屏障
	// 	1, &bufferMemoryBarrier, // 缓冲区屏障
	// 	0, nullptr); // 图像屏障

	auto modelMatrix = modelGameObject->transform->GetModelMatrix();
	applicationEditor->transformGizmo->uniformBuffer->UpdateMVP(m_currentFrame, editorCamera, modelMatrix, projection);
	applicationEditor->transformGizmo->Draw(editorCamera, currentCommandBuffer, mainPipeline->descriptorResource, m_currentFrame, surface->glfwWindow, projection);
	// Draw(glm::vec3(3, 3, 0), material->irradianceMap->irradianceMap, currentCommandBuffer);

	vkCmdEndRenderPass(currentCommandBuffer);

	if (vkEndCommandBuffer(currentCommandBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to record command buffer!");
	}
}


void Application::Draw(const glm::vec3& position, const std::shared_ptr<RenderTexture>& inRenderTexture, const VkCommandBuffer& commandBuffer)
{
	// auto modelMatrix = glm::translate(glm::mat4(1.0f), position);
	//
	// VkDescriptorSetLayoutBinding mvpLayoutBinding;
	// mvpLayoutBinding.binding = 0;
	// mvpLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	// mvpLayoutBinding.descriptorCount = 1;
	// mvpLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	// mvpLayoutBinding.pImmutableSamplers = nullptr; // Optional
	//
	//
	// VkDescriptorSetLayoutBinding renderTextureLayoutBinding;
	// renderTextureLayoutBinding.binding = 1;
	// renderTextureLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	// renderTextureLayoutBinding.descriptorCount = 1;
	// renderTextureLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	// renderTextureLayoutBinding.pImmutableSamplers = nullptr; // Optional
	// auto vkDescriptorSetLayout = descriptorResource->CreateDescriptorSetLayout({mvpLayoutBinding, renderTextureLayoutBinding});
	// auto vkPipelineLayout = mainPipeline->CreatePipelineLayout(descriptorResource, vkDescriptorSetLayout);
	//
	// std::filesystem::path binariesDir = FileSystemBase::getBinariesDir();
	// auto tempVertexShader = FileSystemBase::readFile((binariesDir / "Shaders/TextureView_vert.spv").string());
	// auto tempFragmentShader = FileSystemBase::readFile((binariesDir / "Shaders/TextureView_frag.spv").string());
	// VkShaderModule vertShaderModule = Pipeline::CreateShaderModule(device, tempVertexShader);
	// VkShaderModule fragShaderModule = Pipeline::CreateShaderModule(device, tempFragmentShader);
	//
	// VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	// vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	// auto bindingDescription = GetBindingDescription();
	// auto attributeDescriptions = GetAttributeDescriptions();
	//
	// vertexInputInfo.vertexBindingDescriptionCount = 1;
	// vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	// vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	// vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
	//
	// VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	// inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	// inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	// inputAssembly.primitiveRestartEnable = VK_FALSE;
	//
	// VkPipelineDepthStencilStateCreateInfo depthStencil{};
	// depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	// depthStencil.depthTestEnable = VK_TRUE;
	// depthStencil.depthWriteEnable = VK_TRUE;
	// depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
	// depthStencil.depthBoundsTestEnable = VK_FALSE;
	// depthStencil.minDepthBounds = 0.0f; // Optional
	// depthStencil.maxDepthBounds = 1.0f; // Optional
	// depthStencil.stencilTestEnable = VK_FALSE;
	// depthStencil.front = {}; // Optional
	// depthStencil.back = {}; // Optional
	// auto vkPipeline = mainPipeline->CreatePipeline(vertShaderModule, fragShaderModule, false, vertexInputInfo, inputAssembly, depthStencil, vkPipelineLayout, renderPass->vkRenderPass, false);
	//
	// vkDestroyShaderModule(device->vkDevice, fragShaderModule, nullptr);
	// vkDestroyShaderModule(device->vkDevice, vertShaderModule, nullptr);
	//
	// std::shared_ptr<UniformBuffers> uniformBuffer = std::make_shared<UniformBuffers>(device);
	// auto descriptorSets = CreateDescriptorSet(vkDescriptorSetLayout, uniformBuffer, descriptorResource, inRenderTexture);
	//
	// // auto commandBuffer = commandResource->BeginSingleTimeCommands();
	// vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipeline);
	// uniformBuffer->UpdateMVP(0, editorCamera, modelMatrix, projection);
	// std::vector<Vertex> vertices;
	//
	// vertices.push_back({glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec3(0, 0, 1), glm::vec2(0.0f, 0.0f)});
	// vertices.push_back({glm::vec3(1.0f, -1.0f, 0.0f), glm::vec3(0, 0, 1), glm::vec2(1.0f, 0.0f)});
	// vertices.push_back({glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(0, 0, 1), glm::vec2(1.0f, 1.0f)});
	// vertices.push_back({glm::vec3(-1.0f, 1.0f, 0.0f), glm::vec3(0, 0, 1), glm::vec2(0.0f, 1.0f)});
	// std::vector<uint16_t> indices = {0, 1, 2, 2, 3, 0};
	//
	// auto tempVertexBuffer = std::make_shared<Buffer>(device, sizeof(Vertex) * vertices.size(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
	//                                                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	// auto tempIndexBuffer = std::make_shared<Buffer>(device, sizeof(uint16_t) * indices.size(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
	//                                                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	// VkBuffer vertexBuffers[] = {tempVertexBuffer->vkBuffer};
	// VkDeviceSize offsets[] = {0};
	// vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
	// vkCmdBindIndexBuffer(commandBuffer, tempIndexBuffer->vkBuffer, 0, VK_INDEX_TYPE_UINT32);
	// vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipelineLayout, 0, 1, &descriptorSets, 0, nullptr);
	// vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
	// // commandResource->EndSingleTimeCommands(commandBuffer);
}


VkDescriptorSet Application::CreateDescriptorSet(const VkDescriptorSetLayout& descriptorSetLayout, const std::shared_ptr<UniformBuffers>& inUniformBuffers,
                                                 const std::shared_ptr<DescriptorResource>& inDescriptorResource, const std::shared_ptr<RenderTexture>& inRenderTexture)
{
	VkDescriptorSet vkDescriptorSet;
	std::vector<VkDescriptorSetLayout> layouts(1, descriptorSetLayout);
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = inDescriptorResource->vkDescriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(1);
	allocInfo.pSetLayouts = layouts.data();
	// vkDescriptorSets.resize(m_device->MAX_FRAMES_IN_FLIGHT);
	if (vkAllocateDescriptorSets(device->vkDevice, &allocInfo, &vkDescriptorSet) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate descriptor sets!");
	}
	VkDescriptorBufferInfo mvpBufferInfo;
	mvpBufferInfo.buffer = inUniformBuffers->mvpObjectBuffers[0]->vkBuffer;
	mvpBufferInfo.offset = 0;
	mvpBufferInfo.range = sizeof(MVPObject);


	VkWriteDescriptorSet mvpWrite{};
	mvpWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	mvpWrite.dstSet = vkDescriptorSet;
	mvpWrite.dstBinding = 0;
	mvpWrite.dstArrayElement = 0;
	mvpWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	mvpWrite.descriptorCount = 1;
	mvpWrite.pBufferInfo = &mvpBufferInfo;
	mvpWrite.pImageInfo = nullptr; // Optional
	mvpWrite.pTexelBufferView = nullptr; // Optional


	VkDescriptorImageInfo textureImageInfo;
	textureImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	textureImageInfo.imageView = inRenderTexture->vkImageView;
	textureImageInfo.sampler = inRenderTexture->vkSampler;

	VkWriteDescriptorSet textureMapWrite{};
	textureMapWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	textureMapWrite.dstSet = vkDescriptorSet;
	textureMapWrite.dstBinding = 1;
	textureMapWrite.dstArrayElement = 0;
	textureMapWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	textureMapWrite.descriptorCount = 1;
	textureMapWrite.pImageInfo = &textureImageInfo;

	std::array descriptorWrites{mvpWrite, textureMapWrite};
	vkUpdateDescriptorSets(device->vkDevice, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	return vkDescriptorSet;
}
