#pragma once
#include "Image.hpp"

namespace Sandbox::Resource
{
    class StbImage : public Resource::Image
    {
    public:
        StbImage(const std::string& name, const std::vector<uint8_t>& data, bool flipOnLoad);
    };
}  // namespace Sandbox::Resource
