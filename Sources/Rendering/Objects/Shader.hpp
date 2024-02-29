#pragma once
#include <map>
#include <memory>
#include <string>
#include <glslang/Public/ShaderLang.h>
#include <vulkan/vulkan_core.h>

class DescriptorResource;
class Device;
class File;

/**
 * \brief 反射着色器统一缓冲区
 */
struct ShaderUniformReflection
{
    int32_t binding;
    uint32_t bytes;
    int32_t glDefineType;
    int32_t offset;
    uint32_t descriptorCount;
    VkDescriptorType descriptorType;
    VkShaderStageFlags stageFlags;
};

/**
 * \brief 反射着色器统一缓冲块
 */
struct ShaderUniformBlockReflection
{
    int32_t binding;
    uint32_t bytes;
    VkShaderStageFlags stageFlags;
    uint32_t descriptorCount;
    VkDescriptorType descriptorType;
    bool isLayoutPushConstant;
    std::map<std::string, ShaderUniformReflection> uniforms;
};

/**
 * \brief 反射着色器属性
 */
struct ShaderAttributeReflection
{
    uint32_t layoutSet;
    uint32_t layoutLocation;
    uint32_t bytes;
    int32_t glDefineType;
};

/**
 * \brief 着色器类
 */
class Shader
{
    /**
     * \brief 保存统一缓冲区名称反射
     */
    std::map<std::string, ShaderUniformReflection> m_uniforms;
    /**
     * \brief 保存统一缓冲块名称反射
     */
    std::map<std::string, ShaderUniformBlockReflection> m_uniformBlocks;
    /**
     * \brief 保存属性名称反射
     */
    std::map<std::string, ShaderAttributeReflection> m_attributes;
    /**
     * \brief 设备
     */
    std::shared_ptr<Device> m_device;
    /**
     * \brief 保存着色器模块
     */
    std::map<VkShaderStageFlagBits, VkShaderModule> m_shaderModules;

    /**
     * \brief 构造统一缓冲区反射信息
     * \param uniform 统一缓冲区反射对象
     * \param stageFlag 着色器阶段标志
     */
    void LoadUniform(const glslang::TObjectReflection& uniform, VkShaderStageFlagBits stageFlag);

    /**
     * \brief 构造统一缓冲块反射信息
     * \param attribute 统一缓冲块反射对象
     */
    void LoadAttribute(const glslang::TObjectReflection& attribute);

    /**
     * \brief 构造属性反射信息
     * \param uniformBlock 属性反射对象
     * \param stageFlag 着色器阶段标志
     */
    void LoadUniformBlock(const glslang::TObjectReflection& uniformBlock, VkShaderStageFlagBits stageFlag);

    /**
     * \brief 计算字节数
     * \param tType 类型
     * \return 字节数
     */
    uint32_t ComputeBytes(const glslang::TType* tType);

public:
    /**
     * \brief 初始化
     */
    static void Initialize();

    /**
     * \brief 清理资源
     */
    static void Cleanup();

    /**
     * \brief 构造函数
     * \param device 设备
     */
    Shader(const std::shared_ptr<Device>& device);

    /**
     * \brief 析构函数
     */
    ~Shader();

    /**
     * \brief 加载着色器
     * \param glslSource glsl源文件
     * \param preamble define 信息
     * \param stageFlag 着色器阶段标志
     * \return 是否加载成功
     */
    bool LoadShaderForStage(const std::shared_ptr<File>& glslSource, const std::string& preamble, VkShaderStageFlagBits stageFlag);

    /**
     * \brief 获取着色器模块
     * \param stageFlag 着色器阶段标志
     * \return 着色器模块
     */
    VkShaderModule GetShaderModule(VkShaderStageFlagBits stageFlag);

    /**
     * \brief 构造资源描述符
     * \return 资源描述符
     */
    std::shared_ptr<DescriptorResource> CreateDescriptorResource();
};
