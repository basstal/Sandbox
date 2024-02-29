#pragma once
#include <memory>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "Buffers/UniformBuffer.hpp"

class UniformCombinedImage;

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
class SyncObjects;
class Material;
class RendererSettings;
class Camera;
class Timer;
struct MVPObject;
struct Light;
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
     * \brief 是否已经清理
     */
    bool m_cleaned = false;
    /**
         * \brief 创建同步对象
         * \param count 同步对象数量
         */
    void CreateSemaphore(uint32_t count);

    /**
     * \brief 创建围栏
     * \param count 围栏数量
     */
    void CreateFence(uint32_t count);

    /**
         * \brief 重建交换链
         */
    void RecreateSwapchain();

    /**
         * \brief 填充命令缓冲区
        * \param currentCommandBuffer 当前命令缓冲区
        * \param framebuffer 当前帧缓冲区
         */
    void RecordCommandBuffer(VkCommandBuffer currentCommandBuffer, const std::shared_ptr<Framebuffer>& framebuffer);

public:
    /**
     * \brief 构造函数
     */
    Renderer();

    /**
     * \brief 析构函数
     */
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
    /**
     * \brief 主图形管线
     */
    std::shared_ptr<Pipeline> mainPipeline;
    /**
     * \brief 线框模式 图形管线
     */
    std::shared_ptr<Pipeline> nonSolidPipeline;
    /**
     * \brief 描述符资源
     */
    std::vector<std::shared_ptr<UniformBuffer<MVPObject>>> uniformMvpObjectsNonSolid;

    /**
     * \brief 命令资源
     */
    std::shared_ptr<CommandResource> commandResource;
    /**
     * \brief 渲染对象队列
     */
    std::vector<std::shared_ptr<GameObject>> queuedRenderObjects;

    /**
     * \brief MVP 统一缓冲区，数量与 Swapchain::MAX_FRAMES_IN_FLIGHT 相同
     */
    std::vector<std::shared_ptr<UniformBuffer<MVPObject>>> uniformMvpObjects;

    /**
     * \brief 统一图像采样器，数量与 Swapchain::MAX_FRAMES_IN_FLIGHT 相同
     */
    std::vector<std::shared_ptr<UniformCombinedImage>> combinedImages;

    /**
     * \brief 光信息 统一缓冲区
     */
    std::vector<std::shared_ptr<UniformBuffer<Light>>> uniformLights;

    /**
     * \brief image 可用同步对象
     */
    std::vector<VkSemaphore> imageAvailableSemaphores;
    /**
     * \brief 渲染完成同步对象
     */
    std::vector<VkSemaphore> renderFinishedSemaphores;
    /**
     * \brief 游戏渲染完成同步对象
     */
    std::vector<VkSemaphore> gameRenderFinishedSemaphores;
    /**
     * \brief flight 帧间同步对象
     */
    std::vector<VkFence> inFlightFences;

    /**
     * \brief 渲染器设置
     */
    std::shared_ptr<RendererSettings> settings;

    /**
     * \brief 主摄像机
     */
    std::shared_ptr<Camera> mainCamera;

    /**
     * \brief 当前可用的 Swapchain image 下标
     */
    uint32_t swapchainImageIndex;
    /**
     * \brief 当前绘制帧序号
     */
    uint32_t frameFlightIndex = 0;
    /**
     * \brief 清理资源
     */
    void Cleanup();

    /**
     * \brief 初始化
     */
    void Initialize();

    /**
     * \brief 开始绘制一帧
     */
    void BeginDrawFrame();

    /**
     * \brief 结束绘制一帧
     */
    void EndDrawFrame();
};
