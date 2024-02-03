#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan_core.h>

#include "Rendering/Settings.hpp"

class Surface
{
private:
	bool m_framebufferResized = false;
	static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);
	VkInstance m_vkInstance;
	bool m_cleaned = false;
	std::shared_ptr<Settings> m_settings;

public:
	GLFWwindow* glfwWindow;
	VkSurfaceKHR vkSurface;
	Surface(const VkInstance& instance, const std::shared_ptr<Settings>& settings);
	~Surface();
	void Cleanup();
	bool GetFrameBufferResized();
	void SetFrameBufferResized(bool value);
};
