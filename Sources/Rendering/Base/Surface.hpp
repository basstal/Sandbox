#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan_core.h>

#include "Rendering/Settings.hpp"

class Surface
{
private:
	static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);
	void ApplySettings(std::shared_ptr<Settings> settings);
	VkInstance m_vkInstance;
	bool m_cleaned = false;
	std::shared_ptr<Settings> m_settings;

public:
	bool framebufferResized = false;
	GLFWwindow* glfwWindow;
	VkSurfaceKHR vkSurface;
	Surface(const VkInstance& instance, const std::shared_ptr<Settings>& settings);
	~Surface();
	void Cleanup();
};
