#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan_core.h>

class Surface
{
private:
	int32_t m_width = 800;
	int32_t m_height = 600;
	const char* m_title = "Vulkan";
	bool m_framebufferResized = false;
	static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);
	VkInstance m_vkInstance;
	bool m_cleaned = false;

public:
	GLFWwindow* glfwWindow;
	VkSurfaceKHR vkSurface;
	Surface(const VkInstance& instance);
	~Surface();
	void Cleanup();
	bool GetFrameBufferResized();
	void SetFrameBufferResized(bool value);
};
