#pragma once
#include "Base/Device.hpp"
#include "Components/Buffer.hpp"

class UniformBuffers
{
public:
	std::vector<std::shared_ptr<Buffer>> uniformBuffers;
	std::vector<void*> uniformBuffersMapped;
	UniformBuffers(const std::shared_ptr<Device>& device);
};
