#pragma once

#include <filesystem>
#include <vulkan/vulkan.h>

class Device;
class RenderPass;
class Shader;
class DescriptorResource;
class RendererSettings;

/**
 * \brief 管线类
 */
class Pipeline
{
    /**
     * \brief 设备
     */
    std::shared_ptr<Device> m_device;
    /**
     * \brief 渲染通道
     */
    std::shared_ptr<RenderPass> m_renderPass;
    /**
     * \brief 着色器
     */
    std::shared_ptr<Shader> m_shader;
    /**
     * \brief 是否已清理
     */
    bool m_cleaned = false;

public:
    /**
     * \brief 图形管线布局
     */
    VkPipelineLayout vkPipelineLayout;

    /**
     * \brief 图形管线
     */
    VkPipeline vkPipeline;
    /**
     * \brief 管线资源描述符
     */
    std::shared_ptr<DescriptorResource> descriptorResource;

    /**
     * \brief 构造函数
     * \param device    设备
     * \param shader   着色器
     * \param renderPass    渲染通道
     * \param primitiveTopology   图元拓扑
     * \param polygonMode   多边形模式
     */
    Pipeline(const std::shared_ptr<Device>& device, const std::shared_ptr<Shader>& shader, const std::shared_ptr<RenderPass>& renderPass, VkPrimitiveTopology primitiveTopology,
             VkPolygonMode polygonMode);

    /**
     * \brief 析构函数
     */
    ~Pipeline();

    /**
     * \brief 从着色器中间文件 spv 数据创建着色器模型
     * \param device    设备
     * \param spvCode   spv 数据
     * \return  着色器模型
     */
    static VkShaderModule CreateShaderModule(const std::shared_ptr<Device>& device, const std::vector<char>& spvCode);

    /**
     * \brief 清理资源
     */
    void Cleanup();
};
