#include "pch.hpp"

#include "ShaderModule.hpp"

#include "Device.hpp"
#include "FileSystem/Logger.hpp"
#include "Standard/String.hpp"
#include "VulkanRHI/Common/ShaderIncluder.hpp"
#include "VulkanRHI/Common/ShaderSource.hpp"
#include "VulkanRHI/Rendering/PipelineState.hpp"


const std::map<VkShaderStageFlagBits, EShLanguage> STAGE_TO_LANGUAGE = {{VK_SHADER_STAGE_VERTEX_BIT, EShLangVertex},
                                                                        {VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT, EShLangTessControl},
                                                                        {VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT, EShLangTessEvaluation},
                                                                        {VK_SHADER_STAGE_GEOMETRY_BIT, EShLangGeometry},
                                                                        {VK_SHADER_STAGE_FRAGMENT_BIT, EShLangFragment},
                                                                        {VK_SHADER_STAGE_COMPUTE_BIT, EShLangCompute}};

const std::map<glslang::TStorageQualifier, VkDescriptorType> STORAGE_QUALIFIER_TO_DESCRIPTOR_TYPE = {
    {glslang::EvqUniform, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER},
    {glslang::EvqBuffer, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER},
};

const std::map<int32_t, VkDescriptorType> GL_DEFINE_TYPE_TO_DESCRIPTOR_TYPE = {
    {GL_SAMPLER_2D, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER},  // GL_SAMPLER_2D
    {GL_IMAGE_2D, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER},  // GL_IMAGE_2D
    {GL_SAMPLER_2D_MULTISAMPLE, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER},  // GL_SAMPLER_2D_MULTISAMPLE
    {GL_IMAGE_2D_MULTISAMPLE, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER},  // GL_IMAGE_2D_MULTISAMPLE
    {GL_SAMPLER_CUBE, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER},  // GL_SAMPLER_CUBE
    {GL_IMAGE_CUBE, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER},  // GL_IMAGE_CUBE
    {GL_IMAGE_CUBE_MAP_ARRAY, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER},  // GL_IMAGE_CUBE_MAP_ARRAY
};

const std::map<int32_t, VkFormat> GL_DEFINE_TYPE_TO_FORMAT = {
    {GL_FLOAT, VK_FORMAT_R32_SFLOAT},  // GL_FLOAT
    {GL_FLOAT_VEC2, VK_FORMAT_R32G32_SFLOAT},  // GL_FLOAT_VEC2
    {GL_FLOAT_VEC3, VK_FORMAT_R32G32B32_SFLOAT},  // GL_FLOAT_VEC3
    {GL_FLOAT_VEC4, VK_FORMAT_R32G32B32A32_SFLOAT},  // GL_FLOAT_VEC4
    {GL_INT, VK_FORMAT_R32_SINT},  // GL_INT
    {GL_INT_VEC2, VK_FORMAT_R32G32_SINT},  // GL_INT_VEC2
    {GL_INT_VEC3, VK_FORMAT_R32G32B32_SINT},  // GL_INT_VEC3
    {GL_INT_VEC4, VK_FORMAT_R32G32B32A32_SINT},  // GL_INT_VEC4
    {GL_UNSIGNED_INT, VK_FORMAT_R32_UINT},  // GL_UNSIGNED_INT
    {GL_UNSIGNED_INT_VEC2, VK_FORMAT_R32G32_UINT},  // GL_UNSIGNED_INT_VEC2
    {GL_UNSIGNED_INT_VEC3, VK_FORMAT_R32G32B32_UINT},  // GL_UNSIGNED_INT_VEC3
    {GL_UNSIGNED_INT_VEC4, VK_FORMAT_R32G32B32A32_UINT},  // GL_UNSIGNED_INT_VEC4
    {GL_BOOL, VK_FORMAT_R32_SINT},  // GL_BOOL
    {GL_BOOL_VEC2, VK_FORMAT_R32G32_SINT},  // GL_BOOL_VEC2
    {GL_BOOL_VEC3, VK_FORMAT_R32G32B32_SINT},  // GL_BOOL_VEC3
    {GL_BOOL_VEC4, VK_FORMAT_R32G32B32A32_SINT},  // GL_BOOL_VEC4
    {GL_FLOAT_MAT2, VK_FORMAT_R32G32_SFLOAT},  // GL_FLOAT_MAT2
    {GL_FLOAT_MAT3, VK_FORMAT_R32G32B32_SFLOAT},  // GL_FLOAT_MAT3
    {GL_FLOAT_MAT4, VK_FORMAT_R32G32B32A32_SFLOAT},  // GL_FLOAT_MAT4
    {GL_FLOAT_MAT2x3, VK_FORMAT_R32G32_SFLOAT},  // GL_FLOAT_MAT2x3
    {GL_FLOAT_MAT2x4, VK_FORMAT_R32G32_SFLOAT},  // GL_FLOAT_MAT2x4
};

Sandbox::ShaderModule::ShaderModule(const std::shared_ptr<Device>& device, const std::shared_ptr<ShaderSource>& glslSource, VkShaderStageFlagBits inStage)
{
    m_device       = device;
    m_shaderSource = glslSource;
    vkShaderStage  = inStage;
    Compile(glslSource->filePath, glslSource->source, glslSource->preamble, inStage);
}

Sandbox::ShaderModule::~ShaderModule() { Cleanup(); }

bool Sandbox::ShaderModule::IsValid() { return !m_cleaned; }

void Sandbox::ShaderModule::Cleanup()
{
    if (m_cleaned)
    {
        return;
    }
    vkDestroyShaderModule(m_device->vkDevice, vkShaderModule, nullptr);
    m_cleaned = true;
}
void Sandbox::ShaderModule::Recompile()
{
    m_shaderSource->Reload();
    if (vkShaderModule != VK_NULL_HANDLE)
    {
        vkDestroyShaderModule(m_device->vkDevice, vkShaderModule, nullptr);
    }
    Compile(m_shaderSource->filePath, m_shaderSource->source, m_shaderSource->preamble, vkShaderStage);
    onShaderRecompile.Trigger(shared_from_this());
}

void Sandbox::ShaderModule::Compile(const std::string& shaderName, const std::string& inShaderSource, const std::string& preamble, VkShaderStageFlagBits stage)
{
    // TODO:recompile 失败了不崩溃
    if (!STAGE_TO_LANGUAGE.contains(stage))
    {
        LOGF_OLD("Stage {} not supported", std::to_string(stage))
    }
    glslang::TProgram       program;
    glslang::TShader        shader(STAGE_TO_LANGUAGE.at(stage));
    const TBuiltInResource* resource = GetDefaultResources();

    // std::string shaderName       = glslSource.filePath;
    // std::string shaderSource     = glslSource.source;
    const char* shaderNameCStr   = shaderName.c_str();
    const char* shaderSourceCStr = inShaderSource.c_str();
    auto        vulkanVersion    = glslang::EShTargetVulkan_1_3;
    shader.setPreamble(preamble.c_str());
    shader.setStringsWithLengthsAndNames(&shaderSourceCStr, nullptr, &shaderNameCStr, 1);
    shader.setEnvInput(glslang::EShSourceGlsl, shader.getStage(), glslang::EShClientVulkan, 110);
    shader.setEnvClient(glslang::EShClientVulkan, vulkanVersion);
    shader.setEnvTarget(glslang::EShTargetSpv, glslang::EShTargetSpv_1_0);

    EShMessages messages = static_cast<EShMessages>(EShMsgSpvRules | EShMsgVulkanRules | EShMsgDefault);
#if defined(_DEBUG)
    messages = static_cast<EShMessages>(messages | EShMsgDebugInfo);
#endif
    ShaderIncluder includer;
    std::string    outputs;
    if (!shader.preprocess(resource, vulkanVersion, ENoProfile, false, false, messages, &outputs, includer))
    {
        LOGF_OLD("GLSL Preprocessing failed for {} : \n{}\n{}", shaderName, shader.getInfoLog(), shader.getInfoDebugLog())
    }
    if (!shader.parse(resource, vulkanVersion, true, messages, includer))
    {
        LOGF_OLD("GLSL Parsing failed for {} :\n{}\n{}", shaderName, shader.getInfoLog(), shader.getInfoDebugLog())
    }

    program.addShader(&shader);

    if (!program.link(messages) || !program.mapIO())
    {
        LOGF_OLD("GLSL Linking failed for {} :\n{}\n{}", shaderName, program.getInfoLog(), program.getInfoDebugLog())
    }

    program.buildReflection();

    for (int32_t i = 0; i < program.getNumLiveUniformBlocks(); i++)
    {
        auto uniformBlock = program.getUniformBlock(i);
        LoadUniformBlock(uniformBlock, stage);
    }

    for (int32_t i = 0; i < program.getNumLiveUniformVariables(); i++)
    {
        auto uniform = program.getUniform(i);
        LoadUniform(uniform, stage);
    }
    for (int32_t i = 0; i < program.getNumLiveAttributes(); i++)
    {
        // NOTE: 由于 死代码消除（DCE） 机制，无法反射未使用的属性，这会导致 vulkan 报错因为描述符资源无法与 shader 对齐而报错，目前解决方案为修改 shader 删除未使用的属性
        auto attribute = program.getPipeInput(i);
        LoadAttribute(attribute);
    }

    glslang::SpvOptions spvOptions;
#if defined(_DEBUG)
    spvOptions.generateDebugInfo = true;
    spvOptions.disableOptimizer  = true;
    spvOptions.optimizeSize      = false;
#else
    spvOptions.generateDebugInfo = false;
    spvOptions.disableOptimizer  = false;
    spvOptions.optimizeSize      = true;
#endif

    spv::SpvBuildLogger   logger;
    std::vector<uint32_t> spirv;
    glslang::GlslangToSpv(*program.getIntermediate(shader.getStage()), spirv, &logger, &spvOptions);

    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = spirv.size() * sizeof(uint32_t);
    createInfo.pCode    = spirv.data();

    if (vkCreateShaderModule(m_device->vkDevice, &createInfo, nullptr, &vkShaderModule) != VK_SUCCESS)
    {
        LOGF_OLD("Failed to create shader module!")
    }
}

void Sandbox::ShaderModule::SetUniformDescriptorMode(const std::string& uniformName, DescriptorMode inMode)
{
    // TODO: 这里改变了需不需要重载所有引用这个 shaderModule 的 pipeline？
    if (m_uniforms.contains(uniformName))
    {
        auto& uniform = m_uniforms.at(uniformName);
        if (uniform.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER || uniform.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC)
        {
            uniform.descriptorType = inMode == Dynamic ? VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC : VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        }
        if (uniform.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER || uniform.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC)
        {
            uniform.descriptorType = inMode == Dynamic ? VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC : VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        }
    }
    if (m_uniformBlocks.contains(uniformName))
    {
        auto& uniformBlocks = m_uniformBlocks.at(uniformName);
        if (uniformBlocks.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER || uniformBlocks.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC)
        {
            uniformBlocks.descriptorType = inMode == Dynamic ? VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC : VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        }
        if (uniformBlocks.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER || uniformBlocks.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC)
        {
            uniformBlocks.descriptorType = inMode == Dynamic ? VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC : VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        }
    }
}

void Sandbox::ShaderModule::ReflectDescriptorSetLayoutBindings(std::vector<VkDescriptorSetLayoutBinding>&        vkDescriptorSetLayoutBindings,
                                                               std::map<std::string, uint32_t>&                  nameToBinding,
                                                               std::map<uint32_t, VkDescriptorSetLayoutBinding>& bindingToLayoutBinding) const
{
    for (const auto& [uniformBlockName, uniformBlock] : m_uniformBlocks)
    {
        if (!uniformBlock.isLayoutPushConstant)
        {
            nameToBinding[uniformBlockName] = uniformBlock.binding;
            VkDescriptorSetLayoutBinding layoutBinding{};
            layoutBinding.binding            = uniformBlock.binding;
            layoutBinding.descriptorType     = uniformBlock.descriptorType;
            layoutBinding.descriptorCount    = uniformBlock.descriptorCount;
            layoutBinding.stageFlags         = uniformBlock.stageFlags;
            layoutBinding.pImmutableSamplers = nullptr;  // Optional
            vkDescriptorSetLayoutBindings.emplace_back(layoutBinding);
            bindingToLayoutBinding.emplace(uniformBlock.binding, layoutBinding);
        }
    }
    for (const auto& [uniformName, uniform] : m_uniforms)
    {
        nameToBinding[uniformName] = uniform.binding;
        VkDescriptorSetLayoutBinding layoutBinding{};
        layoutBinding.binding            = uniform.binding;
        layoutBinding.descriptorType     = uniform.descriptorType;
        layoutBinding.descriptorCount    = uniform.descriptorCount;
        layoutBinding.stageFlags         = uniform.stageFlags;
        layoutBinding.pImmutableSamplers = nullptr;  // Optional
        vkDescriptorSetLayoutBindings.emplace_back(layoutBinding);
        bindingToLayoutBinding.emplace(uniform.binding, layoutBinding);
    }
}

void Sandbox::ShaderModule::ReflectPushConstantRanges(std::vector<VkPushConstantRange>& vkPushConstantRanges) const
{
    uint32_t pushConstantOffset = 0;
    for (const auto& [uniformBlockName, uniformBlock] : m_uniformBlocks)
    {
        if (uniformBlock.isLayoutPushConstant)
        {
            VkPushConstantRange pushConstantRange{};
            pushConstantRange.stageFlags = uniformBlock.stageFlags;
            pushConstantRange.offset     = pushConstantOffset;
            pushConstantRange.size       = uniformBlock.bytes;
            pushConstantOffset += uniformBlock.bytes;
            vkPushConstantRanges.emplace_back(pushConstantRange);
        }
    }
}

void Sandbox::ShaderModule::ReflectVertexInputState(VertexInputState& vertexInputState) const
{
    uint32_t offset = 0;
    // 按照 layoutLocation 排序
    std::vector<std::pair<std::string, ShaderAttributeReflection>> sortedAttributes{m_attributes.begin(), m_attributes.end()};
    std::sort(sortedAttributes.begin(), sortedAttributes.end(), [](const auto& a, const auto& b) { return a.second.layoutLocation < b.second.layoutLocation; });
    for (const auto& [attributeName, attribute] : sortedAttributes)
    {
        VkVertexInputAttributeDescription vertexInputAttributeDescription{};
        vertexInputAttributeDescription.location = attribute.layoutLocation;
        vertexInputAttributeDescription.binding  = 0;
        if (!GL_DEFINE_TYPE_TO_FORMAT.contains(attribute.glDefineType))
        {
            LOGF_OLD("Attribute {} has unsupported glDefineType {} for VkFormat", attributeName, std::to_string(attribute.glDefineType))
        }
        vertexInputAttributeDescription.format = GL_DEFINE_TYPE_TO_FORMAT.at(attribute.glDefineType);
        vertexInputAttributeDescription.offset = offset;
        offset += attribute.bytes;
        vertexInputState.attributes.emplace_back(vertexInputAttributeDescription);
    }

    assert(vertexInputState.bindings.empty());  // TODO:目前仅支持单个 binding（即 binding 为 0 的情况）
    vertexInputState.bindings.emplace_back();
    vertexInputState.bindings[0].binding   = 0;
    vertexInputState.bindings[0].stride    = offset;
    vertexInputState.bindings[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
}

std::shared_ptr<Sandbox::Device> Sandbox::ShaderModule::GetDevice() { return m_device; }

void Sandbox::ShaderModule::LoadUniform(const glslang::TObjectReflection& uniform, VkShaderStageFlagBits stage)
{
    auto                    uniformType = uniform.getType();
    ShaderUniformReflection u;
    u.binding         = uniform.getBinding();
    u.bytes           = ComputeBytes(uniformType);
    u.glDefineType    = uniform.glDefineType;
    u.offset          = uniform.offset;
    u.stageFlags      = stage;
    u.descriptorCount = uniform.topLevelArrayStride > 0 ? uniform.topLevelArrayStride : 1;
    if (uniform.getBinding() == -1)
    {
        auto splitResult = String::Split(uniform.name, '.');
        if (splitResult.size() <= 1)
        {
            LOGF_OLD("Uniform {} has no binding", uniform.name)
        }
        auto uniformBlockName  = splitResult[0];
        auto uniformName       = String::Replace(uniform.name, uniformBlockName + ".", "");
        auto existUniformBlock = m_uniformBlocks.find(uniformBlockName);
        if (existUniformBlock != m_uniformBlocks.end())
        {
            u.descriptorType                                = VK_DESCRIPTOR_TYPE_MAX_ENUM;
            existUniformBlock->second.uniforms[uniformName] = u;
            return;
        }
        LOGF("VulkanRHI", "Uniform block {} not found for uniform {}", uniformBlockName, uniform.name)
    }
    auto existUniform = m_uniforms.find(uniform.name);
    if (existUniform != m_uniforms.end())
    {
        existUniform->second.stageFlags |= stage;
        return;
    }
    if (uniform.glDefineType != 0 && !GL_DEFINE_TYPE_TO_DESCRIPTOR_TYPE.contains(uniform.glDefineType))
    {
        LOGF("VulkanRHI", "Uniform {} has unsupported glDefineType {} for VkDescriptorType", uniform.name, std::to_string(uniform.glDefineType))
    }
    if (uniform.glDefineType == 0)
    {
        auto uniformQualifier = uniformType->getQualifier();
        if (uniformQualifier.isUniform() && uniformQualifier.layoutAttachment >= 0)  // ??
        {
            u.descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
        }
    }
    else
    {
        u.descriptorType = GL_DEFINE_TYPE_TO_DESCRIPTOR_TYPE.at(uniform.glDefineType);
    }
    m_uniforms[uniform.name] = u;
}

void Sandbox::ShaderModule::LoadAttribute(const glslang::TObjectReflection& attribute)
{
    if (attribute.name.empty())
    {
        return;  // do not add empty named attributes
    }
    auto existAttribute = m_attributes.find(attribute.name);
    if (existAttribute != m_attributes.end())
    {
        return;
    }
    auto&                     qualifier = attribute.getType()->getQualifier();
    ShaderAttributeReflection a;
    a.layoutSet                  = qualifier.layoutSet;
    a.layoutLocation             = qualifier.layoutLocation;
    a.bytes                      = ComputeBytes(attribute.getType());
    a.glDefineType               = attribute.glDefineType;
    m_attributes[attribute.name] = a;
}

void Sandbox::ShaderModule::LoadUniformBlock(const glslang::TObjectReflection& uniformBlock, VkShaderStageFlagBits stage)
{
    auto existUniformBlock = m_uniformBlocks.find(uniformBlock.name);
    if (existUniformBlock != m_uniformBlocks.end())
    {
        existUniformBlock->second.stageFlags |= stage;
        return;
    }
    ShaderUniformBlockReflection block;
    block.binding    = uniformBlock.getBinding();
    block.bytes      = ComputeBytes(uniformBlock.getType());
    block.stageFlags = stage;
    auto& qualifier  = uniformBlock.getType()->getQualifier();
    if (!STORAGE_QUALIFIER_TO_DESCRIPTOR_TYPE.contains(qualifier.storage))
    {
        LOGF_OLD("Uniform block {} has unsupported storage {} for VkDescriptorType", uniformBlock.name, std::to_string(qualifier.storage))
    }
    block.descriptorCount              = uniformBlock.topLevelArrayStride > 0 ? uniformBlock.topLevelArrayStride : 1;
    block.descriptorType               = STORAGE_QUALIFIER_TO_DESCRIPTOR_TYPE.at(qualifier.storage);
    block.isLayoutPushConstant         = qualifier.layoutPushConstant;
    m_uniformBlocks[uniformBlock.name] = block;
}

uint32_t Sandbox::ShaderModule::ComputeBytes(const glslang::TType* tType)
{
    uint32_t size = 0;
    if (tType->getBasicType() == glslang::EbtBlock || tType->getBasicType() == glslang::EbtStruct)
    {
        for (const auto& internalStruct : *tType->getStruct())
        {
            size += ComputeBytes(internalStruct.type);
        }
    }
    else if (tType->isMatrix())
    {
        size = static_cast<uint32_t>(static_cast<uint64_t>(tType->getMatrixCols()) * tType->getMatrixRows() * sizeof(float));
    }
    else
    {
        size = tType->getVectorSize() * sizeof(float);
    }

    if (tType->getArraySizes())
    {
        uint32_t arraySize = 1;
        for (int32_t i = 0; i < tType->getArraySizes()->getNumDims(); i++)
        {
            auto dimSize = std::max(1, tType->getArraySizes()->getDimSize(i));
            arraySize *= dimSize;
        }
        size *= arraySize;
    }
    return size;
}
