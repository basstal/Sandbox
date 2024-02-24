#pragma once
#include "CubeMap.hpp"
#include "Objects/RenderTexture.hpp"

class Material
{
private:
	bool m_cleaned = false;

public:
	int irradianceMapSize = 512;
	void Cleanup();
	void TransitionImageLayout();
	~Material();
	Material(const std::shared_ptr<Device>& device, const std::shared_ptr<Image>& albedoImage, const std::shared_ptr<Image>& metallicImage, const std::shared_ptr<Image>& roughnessImage,
	         const std::shared_ptr<Image>& aoImage, const std::shared_ptr<Image>& irradianceImage, const std::shared_ptr<CommandResource>& commandResource, const std::shared_ptr<Pipeline>& pipeline,
	         const std::shared_ptr<RenderPass>& renderPass, const std::shared_ptr<DescriptorResource>& descriptorResource);
	std::shared_ptr<RenderTexture> albedoMap;
	std::shared_ptr<RenderTexture> metallicMap;
	std::shared_ptr<RenderTexture> roughnessMap;
	std::shared_ptr<RenderTexture> aoMap;

	std::shared_ptr<CubeMap> irradianceMap;
	// TODO: 
	std::shared_ptr<RenderTexture> normalMap;
	std::shared_ptr<RenderTexture> heightMap;
	std::shared_ptr<RenderTexture> emissiveMap;
	// std::shared_ptr<RenderTexture> brdfLUT;
	std::shared_ptr<RenderTexture> prefilteredMap;
	std::shared_ptr<RenderTexture> lutMap;
	std::shared_ptr<RenderTexture> depthMap;
	std::shared_ptr<RenderTexture> shadowMap;
	std::shared_ptr<RenderTexture> ssaoMap;
	std::shared_ptr<RenderTexture> ssrMap;
	std::shared_ptr<RenderTexture> bloomMap;
	std::shared_ptr<RenderTexture> motionBlurMap;
	std::shared_ptr<RenderTexture> ditherMap;
	std::shared_ptr<RenderTexture> colorMap;
	std::shared_ptr<RenderTexture> velocityMap;
	std::shared_ptr<RenderTexture> distortionMap;
	std::shared_ptr<RenderTexture> lightMap;
};
