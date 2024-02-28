#pragma once
#include "CubeMap.hpp"
#include "Objects/Framebuffer.hpp"

class Material
{
private:
    bool m_cleaned = false;

public:
    int irradianceMapSize = 512;

    void Cleanup();

    void TransitionImageLayout();

    void UpdateWriteDescriptorSet(const std::shared_ptr<DescriptorResource>& descriptorResource);

    ~Material();

    Material(const std::shared_ptr<Device>& device, const std::shared_ptr<GameCore::Image>& albedoImage, const std::shared_ptr<GameCore::Image>& metallicImage,
             const std::shared_ptr<GameCore::Image>& roughnessImage,
             const std::shared_ptr<GameCore::Image>& aoImage, const std::shared_ptr<GameCore::Image>& irradianceImage, const std::shared_ptr<CommandResource>& commandResource,
             const std::shared_ptr<Pipeline>& pipeline,
             const std::shared_ptr<RenderPass>& renderPass, const std::shared_ptr<DescriptorResource>& descriptorResource);

    std::shared_ptr<Framebuffer> albedoMap;
    std::shared_ptr<Framebuffer> metallicMap;
    std::shared_ptr<Framebuffer> roughnessMap;
    std::shared_ptr<Framebuffer> aoMap;

    std::shared_ptr<CubeMap> irradianceMap;
    // TODO:
    std::shared_ptr<Framebuffer> normalMap;
    std::shared_ptr<Framebuffer> heightMap;
    std::shared_ptr<Framebuffer> emissiveMap;
    // std::shared_ptr<RenderTexture> brdfLUT;
    std::shared_ptr<Framebuffer> prefilteredMap;
    std::shared_ptr<Framebuffer> lutMap;
    std::shared_ptr<Framebuffer> depthMap;
    std::shared_ptr<Framebuffer> shadowMap;
    std::shared_ptr<Framebuffer> ssaoMap;
    std::shared_ptr<Framebuffer> ssrMap;
    std::shared_ptr<Framebuffer> bloomMap;
    std::shared_ptr<Framebuffer> motionBlurMap;
    std::shared_ptr<Framebuffer> ditherMap;
    std::shared_ptr<Framebuffer> colorMap;
    std::shared_ptr<Framebuffer> velocityMap;
    std::shared_ptr<Framebuffer> distortionMap;
    std::shared_ptr<Framebuffer> lightMap;
};
