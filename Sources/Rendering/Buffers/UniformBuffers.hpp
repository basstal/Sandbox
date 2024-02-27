#pragma once
#include <memory>

#include "Rendering/Camera.hpp"
#include "Rendering/Model.hpp"
#include "Rendering/Base/Device.hpp"
#include "Rendering/Buffers/Buffer.hpp"
#include "Rendering/Components/DescriptorResource.hpp"

class UniformBuffers
{
private:
	bool m_cleanup = false;
	std::shared_ptr<Device> m_device;

public:
	std::vector<std::shared_ptr<Buffer>> mvpObjectBuffers;
	std::vector<void*> mvpObjectBuffersMapped;

	// std::vector<std::shared_ptr<Buffer>> pbrMaterialBuffers;
	// std::vector<void*> pbrMaterialBuffersMapped;

	std::vector<std::shared_ptr<Buffer>> pbrLightBuffers;
	std::vector<void*> pbrLightBuffersMapped;
	UniformBuffers(const std::shared_ptr<Device>& device);
	~UniformBuffers();
	MVPObject UpdateMVP(uint32_t currentImage, const std::shared_ptr<Camera>& camera, const glm::mat4& model, const glm::mat4& projection);
	MVPObject UpdateMVP(uint32_t currentImage, const glm::mat4& view, const glm::mat4& model, const glm::mat4& projection);
	// void UpdatePBRMaterial(uint32_t currentImage);
	void UpdatePBRLight(uint32_t currentImage, const glm::vec3& position, const glm::vec3& color);
	void UpdateWriteDescriptorSet(const std::shared_ptr<DescriptorResource>& descriptorResource);
	void Cleanup();
};
