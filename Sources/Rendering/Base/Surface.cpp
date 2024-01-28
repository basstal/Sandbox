#include "Surface.hpp"

#include <stdexcept>
#include <vulkan/vulkan_core.h>

Surface::Surface(const VkInstance& instance)
{
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	glfwWindow = glfwCreateWindow(m_width, m_height, m_title, nullptr, nullptr);
	if (glfwWindow == nullptr)
	{
		throw std::runtime_error("failed to create window!");
	}
	glfwSetWindowUserPointer(glfwWindow, this);
	glfwSetFramebufferSizeCallback(glfwWindow, FramebufferResizeCallback);
	if (glfwCreateWindowSurface(instance, glfwWindow, nullptr, &vkSurface) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create window surface!");
	}
}

Surface::~Surface()
{
}

bool Surface::GetFrameBufferResized()
{
	return m_framebufferResized;
}

void Surface::SetFrameBufferResized(bool value)
{
	m_framebufferResized = value;
}

void Surface::FramebufferResizeCallback(GLFWwindow* window, int width, int height)
{
	auto surface = reinterpret_cast<Surface*>(glfwGetWindowUserPointer(window));
	surface->m_framebufferResized = true;
}
