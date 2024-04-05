#include "pch.hpp"

#include "Instance.hpp"

#include "FileSystem/Logger.hpp"


static void CheckExtensionsSupport(uint32_t extensionsCount, const char** extensions)
{
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> vkExtensionPropertiesSet(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, vkExtensionPropertiesSet.data());
    for (uint32_t i = 0; i < extensionsCount; i++)
    {
        bool isContained = false;
        for (const auto& vkExtensionProperties : vkExtensionPropertiesSet)
        {
            if (strcmp(extensions[i], vkExtensionProperties.extensionName) == 0)
            {
                isContained = true;
                break;
            }
        }
        if (!isContained)
        {
            LOGF_OLD("{} is not supported in vkExtensionProperties.", extensions[i])
        }
    }
}


static std::vector<const char*> GetRequiredExtensions()
{
    uint32_t     glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
    CheckExtensionsSupport(glfwExtensionCount, glfwExtensions);

    return extensions;
}

Sandbox::Instance::Instance(uint32_t inApiVersion, const char* applicationName) : apiVersion(inApiVersion)
{
    VkApplicationInfo appInfo{};
    appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName   = applicationName;
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName        = "Sandbox";
    appInfo.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion         = apiVersion;

    auto                 requiredExtensions = GetRequiredExtensions();
    VkInstanceCreateInfo createInfo{};
    createInfo.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo        = &appInfo;
    createInfo.enabledLayerCount       = 0;
    createInfo.pNext                   = nullptr;
    createInfo.enabledExtensionCount   = static_cast<uint32_t>(requiredExtensions.size());
    createInfo.ppEnabledExtensionNames = requiredExtensions.data();

    if (vkCreateInstance(&createInfo, nullptr, &vkInstance) != VK_SUCCESS)
    {
        LOGF_OLD("failed to create instance!")
    }
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
