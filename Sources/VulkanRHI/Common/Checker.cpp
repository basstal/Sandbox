#include "pch.hpp"

#include "Checker.hpp"

bool Sandbox::IsDepthOnlyFormat(VkFormat format)
{
    return format == VK_FORMAT_D16_UNORM || format == VK_FORMAT_D32_SFLOAT;  // only depth
}

bool Sandbox::IsDepthFormat(VkFormat format) { return IsDepthOnlyFormat(format) || IsDepthStencilFormat(format); }

bool Sandbox::IsDepthStencilFormat(VkFormat format)
{
    return format == VK_FORMAT_D16_UNORM_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT || format == VK_FORMAT_D32_SFLOAT_S8_UINT;
}
