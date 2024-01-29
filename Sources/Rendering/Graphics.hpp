// #pragma once
//
// #define GLFW_INCLUDE_VULKAN
// #include <GLFW/glfw3.h>
//
// #define GLM_FORCE_RADIANS
// #define GLM_FORCE_DEPTH_ZERO_TO_ONE
// #include <glm/glm.hpp>
// #include <glm/gtc/matrix_transform.hpp>
//
// #include "vulkan/vulkan_core.h"
// #include <chrono>
//
// #include <iostream>
// #include <stdexcept>
// #include <cstdlib>
// #include <vector>
// #include <optional>
// #include <map>
// #include <set>
// #include <cstdint>   // Necessary for uint32_t
// #include <limits>    // Necessary for std::numeric_limits
// #include <algorithm> // Necessary for std::clamp
// #include <fstream>
// #include <filesystem>
// #include <array>
//
// #include "Application.hpp"
// #include "NativeFileSystem.hpp"
// #include "Mesh.hpp"
// #include "Camera.hpp"
// #include "Image.hpp"
// #include "Model.hpp"
// #include "Shared.hpp"
//
// VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
//
//
// void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
//
// class RENDERING_API HelloTriangleApplication
// {
// public:
// 	void run();
//
// 	const int MAX_FRAMES_IN_FLIGHT = 2;
// 	const uint32_t WIDTH = 800;
// 	const uint32_t HEIGHT = 600;
//
// 	const std::vector<const char*> validationLayers = {
// 		"VK_LAYER_KHRONOS_validation"
// 	};
//
//
// 	// NOTE:this could be replaced by using Vulkan Configurator, so it is disabled here.
// 	// const bool enableValidationLayers = false;
// 	std::shared_ptr<Device> m_device;
//
// private:
// 	std::filesystem::path binariesDir;
// 	std::filesystem::path assetsDir;
// 	GLFWwindow* window;
// 	VkInstance instance;
// 	VkDevice device;
// 	VkPhysicalDevice physicalDevice;
// 	VkQueue graphicsQueue;
// 	VkQueue presentQueue;
// 	VkDebugUtilsMessengerEXT debugMessenger;
// 	VkSurfaceKHR surface;
// 	VkSwapchainKHR swapChain;
// 	std::vector<VkImage> swapChainImages;
// 	VkFormat swapChainImageFormat;
// 	VkExtent2D swapChainExtent;
// 	VkRenderPass renderPass;
// 	VkDescriptorSetLayout descriptorSetLayout;
// 	VkPipelineLayout pipelineLayout;
// 	VkPipeline graphicsPipeline;
//
// 	std::vector<VkImageView> swapChainImageViews;
// 	std::vector<VkFramebuffer> swapChainFramebuffers;
//
// 	VkCommandPool commandPool;
// 	std::vector<VkCommandBuffer> commandBuffers;
//
// 	std::vector<VkSemaphore> imageAvailableSemaphores;
// 	std::vector<VkSemaphore> renderFinishedSemaphores;
// 	std::vector<VkFence> inFlightFences;
//
// 	VkBuffer vertexBuffer;
// 	VkDeviceMemory vertexBufferMemory;
// 	VkBuffer indexBuffer;
// 	VkDeviceMemory indexBufferMemory;
// 	VkDescriptorPool descriptorPool;
// 	std::vector<VkDescriptorSet> descriptorSets;
// 	VkImage textureImage;
// 	VkDeviceMemory textureImageMemory;
// 	VkImageView textureImageView;
// 	VkSampler textureSampler;
//
// 	std::vector<VkBuffer> uniformBuffers;
// 	std::vector<VkDeviceMemory> uniformBuffersMemory;
// 	std::vector<void*> uniformBuffersMapped;
//
// 	VkImage depthImage;
// 	VkDeviceMemory depthImageMemory;
// 	VkImageView depthImageView;
//
// 	uint32_t currentFrame = 0;
// 	std::shared_ptr<Model> model;
// 	std::shared_ptr<Image> image;
// 	VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;
//
// 	VkImage colorImage;
// 	VkDeviceMemory colorImageMemory;
// 	VkImageView colorImageView;
// 	std::unique_ptr<Application> application;
// 	void initGlobalSettings();
// 	void initWindow();
// 	void initVulkan();
//
// 	void createColorResources();
//
//
// 	void loadModel();
//
// 	void createDepthResources();
//
// 	bool hasStencilComponent(VkFormat format);
//
// 	void createTextureSampler();
//
// 	void createTextureImageView();
//
// 	// VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
//
// 	void createTextureImage();
//
// 	void generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);
//
// 	void createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
// 	                 VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
//
// 	void createDescriptorSets();
// 	void createDescriptorPool();
//
// 	void createUniformBuffers();
// 	// void createDescriptorSetLayout();
//
// 	void createVertexBuffer();
//
// 	void createIndexBuffer();
//
// 	VkCommandBuffer beginSingleTimeCommands();
//
// 	void endSingleTimeCommands(VkCommandBuffer commandBuffer);
// 	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
//
// 	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);
//
// 	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
//
// 	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
//
// 	// uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
// 	void createSyncObjects();
//
// 	void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
// 	void createCommandBuffers();
//
// 	void createCommandPool();
// 	void createFramebuffers();
// 	// void createGraphicsPipeline();
//
// 	VkShaderModule createShaderModule(const std::vector<char>& code);
//
// 	void recreateSwapChain();
//
// 	void cleanupSwapChain();
// 	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
// 	void setupDebugMessenger();
//
// 	// void createLogicalDevice();
//
//
// 	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
//
//
// 	void mainLoop();
// 	void drawFrame();
//
// 	void updateUniformBuffer(uint32_t currentImage);
// 	void cleanup();
//
// 	bool checkValidationLayerSupport();
//
// 	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
// 		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
// 		VkDebugUtilsMessageTypeFlagsEXT messageType,
// 		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
// 		void* pUserData);
// 	static std::vector<char> readFile(const std::string& filename);
// };
