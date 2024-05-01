#pragma once
#include "Image.hpp"

namespace Sandbox::Resource
{

    class KtxImage : public Image
    {
    public:
        KtxImage(const std::string& inName, const std::vector<uint8_t>& data);

        uint32_t depth;
        VkFormat format;
    };
}  // namespace Sandbox::Resource
