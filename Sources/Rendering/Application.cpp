#include "Application.hpp"

#include <filesystem>
#include <memory>
#include <stdexcept>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>

#include "Image.hpp"
#include "Model.hpp"
#include "NativeFileSystem.hpp"
#include "Components/RenderPass.hpp"
#include "Base/Device.hpp"
#include "Components/CommandPool.hpp"


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

Application::Application()
{
	glfwInit();

	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Hello Triangle";
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
	surface = std::make_shared<Surface>(vkInstance);
	device = std::make_shared<Device>(vkInstance, surface);
	CreateSwapchain();
	renderPass = std::make_shared<RenderPass>(device, swapchain);
	descriptorSet = std::make_shared<DescriptorSet>(device);
	pipeline = std::make_shared<Pipeline>(device, descriptorSet, renderPass);
	std::filesystem::path binariesDir = FileSystemBase::getBinariesDir();
	std::vector<char> vertexShader = FileSystemBase::readFile((binariesDir / "Shaders/Test_vert.spv").string());
	std::vector<char> fragmentShader = FileSystemBase::readFile((binariesDir / "Shaders/Test_frag.spv").string());
	pipeline->CreatePipeline(vertexShader, fragmentShader);
	swapchain->CreateFramebuffers(renderPass);
	commandPool = std::make_shared<CommandPool>(device);
	std::filesystem::path assetsDir = FileSystemBase::getAssetsDir();

	auto model = Model::loadModel((assetsDir / "Models/viking_room.obj").string().c_str());
	auto image = Image::loadImage((assetsDir / "Textures/viking_room.png").string().c_str());
}

Application::~Application()
{
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
