#pragma once

#include <vector>
#include <vulkan/vulkan_core.h>

#include "VulkanRHI/Common/Macros.hpp"

namespace Sandbox
{
    class Instance
    {
    public:
        Instance(uint32_t inApiVersion, const char* applicationName);

        ~Instance();

        DISABLE_COPY_AND_MOVE(Instance)


        void Cleanup();

        VkInstance vkInstance;

        uint32_t apiVersion;

    private:
        bool m_cleaned = false;
    };
}  // namespace Sandbox
