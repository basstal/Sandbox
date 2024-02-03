#include "Surface.hpp"

#include <memory>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

#include "Rendering/Settings.hpp"

Surface::Surface(const VkInstance& instance, const std::shared_ptr<Settings>& settings)
{
	m_settings = settings;
	m_vkInstance = instance;
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	// 获取一个指向用户的主监视器的指针
	GLFWmonitor* monitor = settings->IsWindow ? nullptr : glfwGetPrimaryMonitor();
	// 获取该监视器的视频模式，特别是为了得到屏幕的分辨率。你可以使用glfwGetVideoMode来获取当前视频模式：
	const GLFWvidmode* mode = settings->IsWindow ? nullptr : glfwGetVideoMode(monitor);
	int width = settings->IsWindow ? (int)settings->Width : mode->width;
	int height = settings->IsWindow ? (int)settings->Height : mode->height;
	// 创建窗口
	glfwWindow = glfwCreateWindow(width, height, settings->ApplicationName.c_str(), monitor, nullptr);
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
	Cleanup();
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
	const auto surface = static_cast<Surface*>(glfwGetWindowUserPointer(window));
	surface->m_framebufferResized = true;
	surface->m_settings->Width = width;
	surface->m_settings->Height = height;
}

void Surface::Cleanup()
{
	if (m_cleaned)
	{
		return;
	}
	vkDestroySurfaceKHR(m_vkInstance, vkSurface, nullptr);
	glfwDestroyWindow(glfwWindow);
	m_cleaned = true;
}
