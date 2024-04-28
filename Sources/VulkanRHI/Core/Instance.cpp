#include "pch.hpp"

#include "Instance.hpp"

#include "FileSystem/Logger.hpp"
#include "VulkanRHI/Common/Debug.hpp"


static std::vector<const char*> GetRequiredExtensions()
{
    uint32_t     glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
    return extensions;
}


void Sandbox::Instance::IsExtensionsSupported(const std::vector<const char*>& inExtensions)
{
    uint32_t extensionCount = 0;
    ValidateVkResult(vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr));
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    ValidateVkResult(vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableExtensions.data()));

    for (const char* extension : inExtensions)
    {
        bool found = false;
        for (const auto& availableExtension : availableExtensions)
        {
            if (strcmp(extension, availableExtension.extensionName) == 0)
            {
                found = true;
                break;
            }
        }
        if (!found)
        {
            LOGF("VulkanRHI", "{} extension is not supported!", extension)
        }
    }
}

Sandbox::Instance::Instance(uint32_t inApiVersion, const char* applicationName, const std::vector<const char*>& inExtensions) : apiVersion(inApiVersion)
{
    auto requiredExtensions = GetRequiredExtensions();
    for (auto& extension : inExtensions)
    {
        requiredExtensions.push_back(extension);
    }
    IsExtensionsSupported(requiredExtensions);

    VkApplicationInfo appInfo{};
    appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName   = applicationName;
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName        = "Sandbox";
    appInfo.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion         = apiVersion;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo        = &appInfo;
    createInfo.enabledLayerCount       = 0;
    createInfo.pNext                   = nullptr;
    createInfo.enabledExtensionCount   = static_cast<uint32_t>(requiredExtensions.size());
    createInfo.ppEnabledExtensionNames = requiredExtensions.data();

    ValidateVkResult(vkCreateInstance(&createInfo, nullptr, &vkInstance));
}
Sandbox::Instance::~Instance() { Cleanup(); }

void Sandbox::Instance::Cleanup()
{
    if (m_cleaned)
    {
        return;
    }
    vkDestroyInstance(vkInstance, nullptr);
    m_cleaned = true;
}
