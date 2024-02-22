#pragma once
#include <memory>

#include "Rendering/Camera.hpp"
#include "Rendering/Model.hpp"
#include "Rendering/Base/Device.hpp"
#include "Rendering/Components/Buffer.hpp"

class UniformBuffers
{
private:
	bool m_cleanup = false;

public:
	std::vector<std::shared_ptr<Buffer>> uniformBuffers;
	std::vector<void*> uniformBuffersMapped;
	UniformBuffers(const std::shared_ptr<Device>& device);
	~UniformBuffers();
	UniformBufferObject UpdateUniformBuffer(uint32_t currentImage, const std::shared_ptr<Camera>& camera, const glm::mat4& model, const glm::mat4& projection);
	void Cleanup();
};
