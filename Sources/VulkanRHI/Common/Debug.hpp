#pragma once
#include "FileSystem/Logger.hpp"
#include "Misc/Debug.hpp"
#include "vulkan/vulkan_core.h"

namespace Sandbox
{
    inline void ValidateVkResult(VkResult result)
    {
        switch (result)  // NOLINT(clang-diagnostic-switch-enum)
        {
            case VK_SUCCESS:
                // 操作成功完成
                break;
            case VK_NOT_READY:
                LOGF_OLD("VK_NOT_READY: A fence or query has not yet completed.")
                break;
            case VK_TIMEOUT:
                LOGF_OLD("VK_TIMEOUT: A wait operation has not completed in the specified time.")
                break;
            case VK_EVENT_SET:
                LOGF_OLD("VK_EVENT_SET: An event is signaled.")
                break;
            case VK_EVENT_RESET:
                LOGF_OLD("VK_EVENT_RESET: An event is unsignaled.")
                break;
            case VK_INCOMPLETE:
                LOGF_OLD("VK_INCOMPLETE: A return array was too small for the result.")
                break;
            case VK_ERROR_OUT_OF_HOST_MEMORY:
                LOGF_OLD("VK_ERROR_OUT_OF_HOST_MEMORY: A host memory allocation has failed.")
                break;
            case VK_ERROR_OUT_OF_DEVICE_MEMORY:
                LOGF_OLD("VK_ERROR_OUT_OF_DEVICE_MEMORY: A device memory allocation has failed.")
                break;
            case VK_ERROR_INITIALIZATION_FAILED:
                LOGF_OLD("VK_ERROR_INITIALIZATION_FAILED: Initialization of an object could not be completed for implementation-specific reasons.")
                break;
            case VK_ERROR_DEVICE_LOST:

                LOGF_OLD("VK_ERROR_DEVICE_LOST: The logical or physical device has been lost.\n{}", GetCallStack())
                break;
            case VK_ERROR_MEMORY_MAP_FAILED:
                LOGF_OLD("VK_ERROR_MEMORY_MAP_FAILED: Mapping of a memory object has failed.")
                break;
            case VK_ERROR_LAYER_NOT_PRESENT:
                LOGF_OLD("VK_ERROR_LAYER_NOT_PRESENT: A requested layer is not present or could not be loaded.")
                break;
            case VK_ERROR_EXTENSION_NOT_PRESENT:
                LOGF_OLD("VK_ERROR_EXTENSION_NOT_PRESENT: A requested extension is not supported.")
                break;
            case VK_ERROR_FEATURE_NOT_PRESENT:
                LOGF_OLD("VK_ERROR_FEATURE_NOT_PRESENT: A requested feature is not supported.")
                break;
            case VK_ERROR_INCOMPATIBLE_DRIVER:
                LOGF_OLD("VK_ERROR_INCOMPATIBLE_DRIVER: The requested version of Vulkan is not supported by the driver or is otherwise incompatible.")
                break;
            case VK_ERROR_TOO_MANY_OBJECTS:
                LOGF_OLD("VK_ERROR_TOO_MANY_OBJECTS: Too many objects of the type have already been created.")
                break;
            case VK_ERROR_FORMAT_NOT_SUPPORTED:
                LOGF_OLD("VK_ERROR_FORMAT_NOT_SUPPORTED: A requested format is not supported on this device.")
                break;
            case VK_ERROR_SURFACE_LOST_KHR:
                LOGF_OLD("VK_ERROR_SURFACE_LOST_KHR: A surface is no longer available.")
                break;
            // 添加你需要处理的其他VkResult值
            default:
                LOGF_OLD("Unhandled VkResult: {}", std::to_string(static_cast<uint32_t>(result)))
        }
    }
}
