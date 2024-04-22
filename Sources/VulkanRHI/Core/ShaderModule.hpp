#pragma once
#include <glslang/Public/ShaderLang.h>
#include <map>
#include <memory>
#include <string>

#include "Misc/Event.hpp"
#include "Standard/Dictionary.hpp"
#include "vulkan/vulkan_core.h"

namespace Sandbox
{
    struct VertexInputState;
    /**
     * \brief 反射着色器统一缓冲区
     */
    struct ShaderUniformReflection
    {
        int32_t            binding;
        uint32_t           bytes;
        int32_t            glDefineType;
        int32_t            offset;
        uint32_t           descriptorCount;
        VkDescriptorType   descriptorType;
        VkShaderStageFlags stageFlags;
    };

    /**
     * \brief 反射着色器统一缓冲块
     */
    struct ShaderUniformBlockReflection
    {
        int32_t                                          binding;
        uint32_t                                         bytes;
        VkShaderStageFlags                               stageFlags;
        uint32_t                                         descriptorCount;
        VkDescriptorType                                 descriptorType;
        bool                                             isLayoutPushConstant;
        Dictionary<std::string, ShaderUniformReflection> uniforms;
    };

    /**
     * \brief 反射着色器属性
     */
    struct ShaderAttributeReflection
    {
        uint32_t layoutSet;
        uint32_t layoutLocation;
        uint32_t bytes;
        int32_t  glDefineType;
    };

    enum DescriptorMode
    {
        Static,
        Dynamic
    };
    class Device;
    class ShaderSource;

    class ShaderModule : public std::enable_shared_from_this<ShaderModule>
    {
        friend class ShaderModuleCaching;
    public:
        ShaderModule(const std::shared_ptr<Device>& device, const std::shared_ptr<ShaderSource>& glslSource, VkShaderStageFlagBits inStage);

        ~ShaderModule();

        bool IsValid();

        void Cleanup();

        void Recompile();

        void Compile(const std::string& shaderName, const std::string& inShaderSource, const std::string& preamble, VkShaderStageFlagBits stage);

        void SetUniformDescriptorMode(const std::string& uniformName, DescriptorMode inMode);

        void ReflectDescriptorSetLayoutBindings(std::vector<VkDescriptorSetLayoutBinding>& vkDescriptorSetLayoutBindings, std::map<std::string, uint32_t>& nameToBinding,
                                                std::map<uint32_t, VkDescriptorSetLayoutBinding>& bindingToLayoutBinding) const;

        void ReflectPushConstantRanges(std::vector<VkPushConstantRange>& vkPushConstantRanges) const;

        void ReflectVertexInputState(VertexInputState& vertexInputState) const;

        std::shared_ptr<Device> GetDevice();

        VkShaderModule vkShaderModule;

        VkShaderStageFlagBits vkShaderStage;


        Dictionary<std::string, ShaderUniformBlockReflection> GetUniformBlocks() { return m_uniformBlocks; }

        Event<const std::shared_ptr<ShaderModule>&> onShaderRecompile;

    private:
        std::shared_ptr<ShaderSource> m_shaderSource;
        /**
         * \brief 构造统一缓冲区反射信息
         * \param uniform 统一缓冲区反射对象
         * \param stage 着色器阶段标志
         */
        void LoadUniform(const glslang::TObjectReflection& uniform, VkShaderStageFlagBits stage);

        /**
         * \brief 构造统一缓冲块反射信息
         * \param attribute 统一缓冲块反射对象
         */
        void LoadAttribute(const glslang::TObjectReflection& attribute);

        /**
         * \brief 构造属性反射信息
         * \param uniformBlock 属性反射对象
         * \param stage 着色器阶段标志
         */
        void LoadUniformBlock(const glslang::TObjectReflection& uniformBlock, VkShaderStageFlagBits stage);

        /**
         * \brief 计算字节数
         * \param tType 类型
         * \return 字节数
         */
        uint32_t ComputeBytes(const glslang::TType* tType);

        std::shared_ptr<Device> m_device;
        bool                    m_cleaned = false;

        /**
         * \brief 保存统一缓冲区名称反射
         */
        Dictionary<std::string, ShaderUniformReflection> m_uniforms;
        /**
         * \brief 保存统一缓冲块名称反射
         */
        Dictionary<std::string, ShaderUniformBlockReflection> m_uniformBlocks;
        /**
         * \brief 保存属性名称反射
         */
        Dictionary<std::string, ShaderAttributeReflection> m_attributes;
    };
}
