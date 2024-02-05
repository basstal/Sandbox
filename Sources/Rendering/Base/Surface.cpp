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
	glfwSetWindowPos(glfwWindow, (int)settings->WindowPositionX, (int)settings->WindowPositionY);
	glfwSetWindowUserPointer(glfwWindow, this);
	glfwSetFramebufferSizeCallback(glfwWindow, FramebufferResizeCallback);
	if (glfwCreateWindowSurface(instance, glfwWindow, nullptr, &vkSurface) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create window surface!");
	}
	auto test1 = DataBinding::Get("Rendering/Settings");
	std::shared_ptr<TDataBinding<std::shared_ptr<Settings>>> settingsBinding = std::dynamic_pointer_cast<TDataBinding<std::shared_ptr<Settings>>>(DataBinding::Get("Rendering/Settings"));
	Delegate<std::shared_ptr<Settings>> bindFunction(
		[this](std::shared_ptr<Settings> settings)
		{
			this->ApplySettings(settings);
		}
	);
	settingsBinding->Bind(bindFunction);
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

	if (settings->IsWindow != currentIsWindow)
	{
		if (settings->IsWindow)
		{
			// 当前是全屏模式，切换到窗口模式
			glfwSetWindowMonitor(glfwWindow, NULL,
			                     (int)settings->WindowPositionX, (int)settings->WindowPositionY,
			                     (int)settings->Width, (int)settings->Height, 0);
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
	if (settings->IsWindow)
	{
		int posX, posY, width, height;
		// 保存当前窗口位置和大小
		glfwGetWindowPos(glfwWindow, &posX, &posY);
		glfwGetWindowSize(glfwWindow, &width, &height);
		if (posX != settings->WindowPositionX || posY != settings->WindowPositionY || width != settings->Width || height != settings->Height)
		{
			glfwSetWindowPos(glfwWindow, (int)settings->WindowPositionX, (int)settings->WindowPositionY);
			glfwSetWindowSize(glfwWindow, (int)settings->Width, (int)settings->Height);
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
