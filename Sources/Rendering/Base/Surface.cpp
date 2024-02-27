#include "Surface.hpp"

#include <memory>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

#include "Infrastructures/DataBinding.hpp"
#include "Rendering/Settings.hpp"

Surface::Surface(const VkInstance& instance, const std::shared_ptr<Settings>& settings)
{
	m_settings = settings;
	m_vkInstance = instance;
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	// 获取一个指向用户的主监视器的指针
	GLFWmonitor* monitor = settings->settingsConfig.IsWindow ? nullptr : glfwGetPrimaryMonitor();
	// 获取该监视器的视频模式，特别是为了得到屏幕的分辨率。你可以使用glfwGetVideoMode来获取当前视频模式：
	const GLFWvidmode* mode = settings->settingsConfig.IsWindow ? nullptr : glfwGetVideoMode(monitor);
	int width = settings->settingsConfig.IsWindow ? (int)settings->settingsConfig.Width : mode->width;
	int height = settings->settingsConfig.IsWindow ? (int)settings->settingsConfig.Height : mode->height;
	// 创建窗口
	glfwWindow = glfwCreateWindow(width, height, settings->settingsConfig.ApplicationName.c_str(), monitor, nullptr);
	if (glfwWindow == nullptr)
	{
		throw std::runtime_error("failed to create window!");
	}
	glfwSetWindowPos(glfwWindow, (int)settings->settingsConfig.WindowPositionX, (int)settings->settingsConfig.WindowPositionY);
	glfwSetWindowUserPointer(glfwWindow, this);
	glfwSetFramebufferSizeCallback(glfwWindow, FramebufferResizeCallback);
	glfwSetWindowPosCallback(glfwWindow, [](GLFWwindow* window, int x, int y)
	{
		auto currentIsWindow = glfwGetWindowMonitor(window) == nullptr;
		if (currentIsWindow)
		{
			auto surface = static_cast<Surface*>(glfwGetWindowUserPointer(window));
			surface->m_settings->settingsConfig.WindowPositionX = x;
			surface->m_settings->settingsConfig.WindowPositionY = y;
		}
	});
	glfwSetWindowSizeCallback(glfwWindow, [](GLFWwindow* window, int width, int height)
	{
		auto surface = static_cast<Surface*>(glfwGetWindowUserPointer(window));
		auto currentIsWindow = glfwGetWindowMonitor(window) == nullptr;
		if (currentIsWindow)
		{
			surface->m_settings->settingsConfig.Width = width;
			surface->m_settings->settingsConfig.Height = height;
		}
	});
	if (glfwCreateWindowSurface(instance, glfwWindow, nullptr, &vkSurface) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create window surface!");
	}
	auto settingsBinding = DataBinding::Get<std::shared_ptr<Settings>>("Rendering/Settings");
	settingsBinding->BindMember<Surface, &Surface::ApplySettings>(this);
}

Surface::~Surface()
{
	Cleanup();
}

void Surface::FramebufferResizeCallback(GLFWwindow* window, int width, int height)
{
	const auto surface = static_cast<Surface*>(glfwGetWindowUserPointer(window));
	surface->framebufferResized = true;
}

void Surface::ApplySettings(std::shared_ptr<Settings> settings)
{
	if (glfwWindow == nullptr)
	{
		return;
	}
	auto currentIsWindow = glfwGetWindowMonitor(glfwWindow) == nullptr;

	if (settings->settingsConfig.IsWindow != currentIsWindow)
	{
		if (settings->settingsConfig.IsWindow)
		{
			// 当前是全屏模式，切换到窗口模式
			glfwSetWindowMonitor(glfwWindow, NULL,
			                     (int)settings->settingsConfig.WindowPositionX, (int)settings->settingsConfig.WindowPositionY,
			                     (int)settings->settingsConfig.Width, (int)settings->settingsConfig.Height, 0);
		}
		else
		{
			// 获取当前视频模式和主监视器
			GLFWmonitor* monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);

			// 切换到全屏模式
			glfwSetWindowMonitor(glfwWindow, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
		}
	}
	if (settings->settingsConfig.IsWindow)
	{
		int posX, posY, width, height;
		// 保存当前窗口位置和大小
		glfwGetWindowPos(glfwWindow, &posX, &posY);
		glfwGetWindowSize(glfwWindow, &width, &height);
		if (posX != settings->settingsConfig.WindowPositionX || posY != settings->settingsConfig.WindowPositionY || width != settings->settingsConfig.Width || height != settings->settingsConfig.Height)
		{
			glfwSetWindowPos(glfwWindow, (int)settings->settingsConfig.WindowPositionX, (int)settings->settingsConfig.WindowPositionY);
			glfwSetWindowSize(glfwWindow, (int)settings->settingsConfig.Width, (int)settings->settingsConfig.Height);
		}
	}
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
