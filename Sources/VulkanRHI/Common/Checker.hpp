#pragma once

namespace Sandbox
{
    bool IsDepthOnlyFormat(VkFormat format);
    bool IsDepthFormat(VkFormat format);
    bool IsDepthStencilFormat(VkFormat format);
}  // namespace Sandbox
