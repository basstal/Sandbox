#pragma once
#include <memory>
#include <vector>
#include <vulkan/vulkan_core.h>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

namespace GameCore
{
    class Image;
}

class Surface;
class Device;
class Swapchain;
class RenderPass;
class DescriptorResource;
class Pipeline;
class CommandResource;
class GameObject;
class Framebuffer;
class Image;
class UniformBuffers;
class SyncObjects;
class Material;
class RendererSettings;
class Camera;
class Timer;
class MVPObject;
class ApplicationEditor;

/**
 * \brief 渲染器类
 */
class Renderer
{
    /**
     * \brief 获得应用程序所需的扩展的名称列表
     * \return
     */
    std::vector<const char*> GetRequiredExtensions();

    /**
     * \brief 检查传入的 glfwExtensions 是否都是被支持的扩展
     * \param glfwExtensionCount
     * \param glfwExtensions
     */
    void CheckExtensionsSupport(uint32_t glfwExtensionCount, const char** glfwExtensions);

    /**
     * \brief 当前绘制帧序号
     */
    uint32_t m_currentFrame = 0;

    /**
     * \brief 是否已经清理
     */
    bool m_cleaned = false;

public:
    Renderer();

    ~Renderer();

    /**
     * \brief vulkan API 版本
     */
    uint32_t vulkanApiVersion = VK_API_VERSION_1_3;
    /**
     * \brief vulkan 实例
     */
    VkInstance vkInstance;
    /**
     * \brief surface 相关信息
     */
    std::shared_ptr<Surface> surface;
    /**
     * \brief device 相关信息
     */
    std::shared_ptr<Device> device;
    /**
     * \brief swapchain 相关信息
     */
    std::shared_ptr<Swapchain> swapchain;
    /**
     * \brief 渲染通道
     */
    std::shared_ptr<RenderPass> renderPass;
    // std::shared_ptr<DescriptorResource> descriptorResource;
    std::shared_ptr<Pipeline> mainPipeline;
    std::shared_ptr<CommandResource> commandResource;
    std::vector<std::shared_ptr<GameObject>> queuedRenderObjects;
    // specific to usage
    // std::shared_ptr<Shader> pbrShader;
    std::shared_ptr<Framebuffer> renderTexture;
    // std::shared_ptr<GameObject> modelGameObject;
    std::shared_ptr<GameCore::Image> image;
    // std::vector<char> vertexShader;
    // std::vector<char> fragmentShader;
    // std::shared_ptr<VertexBuffer> vertexBuffer;
    // std::shared_ptr<IndexBuffer> indexBuffer;
    std::shared_ptr<UniformBuffers> uniformBuffers;
    std::shared_ptr<SyncObjects> syncObjects;
    std::shared_ptr<Material> material;
    std::shared_ptr<RendererSettings> settings;
    std::shared_ptr<Camera> editorCamera;
    std::shared_ptr<Timer> timer;
    glm::vec4 clearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    // MVPObject debugUBO;
    glm::mat4 projection;
    float deltaTime;

    void Cleanup();

    void Initialize();

    void LoadAssets();

    void CreateSwapchain();

    void DrawFrame(const std::shared_ptr<ApplicationEditor>& applicationEditor);

    void RecreateSwapchain(const std::shared_ptr<ApplicationEditor>& editor);

    void RecordCommandBuffer(VkCommandBuffer currentCommandBuffer, uint32_t imageIndex, const std::shared_ptr<ApplicationEditor>& applicationEditor);

    VkDescriptorSet CreateDescriptorSet(const VkDescriptorSetLayout& descriptorSetLayout, const std::shared_ptr<UniformBuffers>& inUniformBuffers,
                                        const std::shared_ptr<DescriptorResource>& inDescriptorResource, const std::shared_ptr<Framebuffer>& inRenderTexture);

    void Draw(const glm::vec3& position, const std::shared_ptr<Framebuffer>& inRenderTexture, const VkCommandBuffer& commandBuffer);
};
