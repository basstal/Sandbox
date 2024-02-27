#include "Shader.hpp"
#include <glslang/Public/ShaderLang.h>
#include <glslang/Public/ResourceLimits.h>
#include <glslang/Include/Types.h>
#include <glslang/SPIRV/SpvTools.h>
#include <glslang/SPIRV/GlslangToSpv.h>
#include <glslang/MachineIndependent/gl_types.h>

#include "ShaderIncluder.hpp"
#include "Infrastructures/String.hpp"
#include "Infrastructures/FileSystem/File.hpp"
#include "Rendering/Base/Device.hpp"
#include "Rendering/Components/DescriptorResource.hpp"

const std::map<VkShaderStageFlagBits, EShLanguage> STAGE_TO_LANGUAGE = {
	{VK_SHADER_STAGE_VERTEX_BIT, EShLangVertex},
	{VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT, EShLangTessControl},
	{VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT, EShLangTessEvaluation},
	{VK_SHADER_STAGE_GEOMETRY_BIT, EShLangGeometry},
	{VK_SHADER_STAGE_FRAGMENT_BIT, EShLangFragment},
	{VK_SHADER_STAGE_COMPUTE_BIT, EShLangCompute}
};
const std::map<glslang::TStorageQualifier, VkDescriptorType> STORAGE_QUALIFIER_TO_DESCRIPTOR_TYPE = {
	{glslang::EvqUniform, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER},
	{glslang::EvqBuffer, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER},
};

const std::map<int32_t, VkDescriptorType> GL_DEFINE_TYPE_TO_DESCRIPTOR_TYPE = {
	{GL_SAMPLER_2D, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER}, // GL_SAMPLER_2D
	{GL_IMAGE_2D, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER}, // GL_IMAGE_2D
	{GL_SAMPLER_2D_MULTISAMPLE, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER}, // GL_SAMPLER_2D_MULTISAMPLE
	{GL_IMAGE_2D_MULTISAMPLE, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER}, // GL_IMAGE_2D_MULTISAMPLE
	{GL_SAMPLER_CUBE, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER}, // GL_SAMPLER_CUBE
	{GL_IMAGE_CUBE, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER}, // GL_IMAGE_CUBE
	{GL_IMAGE_CUBE_MAP_ARRAY, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER}, // GL_IMAGE_CUBE_MAP_ARRAY
};

const std::map<int32_t, VkFormat> GL_DEFINE_TYPE_TO_FORMAT = {
	{GL_FLOAT, VK_FORMAT_R32_SFLOAT}, // GL_FLOAT
	{GL_FLOAT_VEC2, VK_FORMAT_R32G32_SFLOAT}, // GL_FLOAT_VEC2
	{GL_FLOAT_VEC3, VK_FORMAT_R32G32B32_SFLOAT}, // GL_FLOAT_VEC3
	{GL_FLOAT_VEC4, VK_FORMAT_R32G32B32A32_SFLOAT}, // GL_FLOAT_VEC4
	{GL_INT, VK_FORMAT_R32_SINT}, // GL_INT
	{GL_INT_VEC2, VK_FORMAT_R32G32_SINT}, // GL_INT_VEC2
	{GL_INT_VEC3, VK_FORMAT_R32G32B32_SINT}, // GL_INT_VEC3
	{GL_INT_VEC4, VK_FORMAT_R32G32B32A32_SINT}, // GL_INT_VEC4
	{GL_UNSIGNED_INT, VK_FORMAT_R32_UINT}, // GL_UNSIGNED_INT
	{GL_UNSIGNED_INT_VEC2, VK_FORMAT_R32G32_UINT}, // GL_UNSIGNED_INT_VEC2
	{GL_UNSIGNED_INT_VEC3, VK_FORMAT_R32G32B32_UINT}, // GL_UNSIGNED_INT_VEC3
	{GL_UNSIGNED_INT_VEC4, VK_FORMAT_R32G32B32A32_UINT}, // GL_UNSIGNED_INT_VEC4
	{GL_BOOL, VK_FORMAT_R32_SINT}, // GL_BOOL
	{GL_BOOL_VEC2, VK_FORMAT_R32G32_SINT}, // GL_BOOL_VEC2
	{GL_BOOL_VEC3, VK_FORMAT_R32G32B32_SINT}, // GL_BOOL_VEC3
	{GL_BOOL_VEC4, VK_FORMAT_R32G32B32A32_SINT}, // GL_BOOL_VEC4
	{GL_FLOAT_MAT2, VK_FORMAT_R32G32_SFLOAT}, // GL_FLOAT_MAT2
	{GL_FLOAT_MAT3, VK_FORMAT_R32G32B32_SFLOAT}, // GL_FLOAT_MAT3
	{GL_FLOAT_MAT4, VK_FORMAT_R32G32B32A32_SFLOAT}, // GL_FLOAT_MAT4
	{GL_FLOAT_MAT2x3, VK_FORMAT_R32G32_SFLOAT}, // GL_FLOAT_MAT2x3
	{GL_FLOAT_MAT2x4, VK_FORMAT_R32G32_SFLOAT}, // GL_FLOAT_MAT2x4
};
Shader::Shader(const std::shared_ptr<Device>& device)
{
	m_device = device;
}
Shader::~Shader()
{
}


void Shader::LoadUniform(const glslang::TProgram& program, VkShaderStageFlagBits stageFlag, int32_t i)
{
	auto uniform = program.getUniform(i);
	Uniform u;
	u.binding = uniform.getBinding();
	u.bytes = ComputeBytes(uniform.getType());
	u.glDefineType = uniform.glDefineType;
	u.offset = uniform.offset;
	u.stageFlags = stageFlag;
	u.descriptorCount = uniform.topLevelArrayStride > 0 ? uniform.topLevelArrayStride : 1;
	if (uniform.getBinding() == -1)
	{
		auto splitResult = String::Split(uniform.name, '.');
		if (splitResult.size() <= 1)
		{
			throw std::runtime_error("Uniform " + uniform.name + " has no binding");
		}
		auto uniformBlockName = splitResult[0];
		auto uniformName = String::Replace(uniform.name, uniformBlockName + ".", "");
		auto existUniformBlock = m_uniformBlocks.find(uniformBlockName);
		if (existUniformBlock != m_uniformBlocks.end())
		{
			u.descriptorType = VK_DESCRIPTOR_TYPE_MAX_ENUM;
			existUniformBlock->second.uniforms[uniformName] = u;
			return;
		}
		throw std::runtime_error("Uniform block " + uniformBlockName + " not found for uniform " + uniform.name);
	}
	auto existUniform = m_uniforms.find(uniform.name);
	if (existUniform != m_uniforms.end())
	{
		existUniform->second.stageFlags |= stageFlag;
		return;
	}
	if (!GL_DEFINE_TYPE_TO_DESCRIPTOR_TYPE.contains(uniform.glDefineType))
	{
		throw std::runtime_error("Uniform " + uniform.name + " has unsupported glDefineType " + std::to_string(uniform.glDefineType) + " for VkDescriptorType");
	}
	u.descriptorType = GL_DEFINE_TYPE_TO_DESCRIPTOR_TYPE.at(uniform.glDefineType);
	m_uniforms[uniform.name] = u;
}
void Shader::LoadAttribute(const glslang::TProgram& program, int32_t i)
{
	auto attribute = program.getPipeInput(i);

	if (attribute.name.empty())
	{
		return; // do not add empty named attributes
	}
	auto existAttribute = m_attributes.find(attribute.name);
	if (existAttribute != m_attributes.end())
	{
		return;
	}
	auto& qualifier = attribute.getType()->getQualifier();
	Attribute a;
	a.layoutSet = qualifier.layoutSet;
	a.layoutLocation = qualifier.layoutLocation;
	a.bytes = ComputeBytes(attribute.getType());
	a.glDefineType = attribute.glDefineType;
	m_attributes[attribute.name] = a;
}
// void Shader::LoadConstant(const glslang::TProgram& program, VkShaderStageFlags stageFlags, int32_t i)
// {
// 	auto constant = program.getConstant(i);
// 	auto existConstant = m_constants.find(constant.name);
// 	if (existConstant != m_constants.end())
// 	{
// 		existConstant->second.stageFlags |= stageFlags;
// 		return;
// 	}
// 	Constant c;
// 	c.size = constant.size;
// 	c.stageFlags = stageFlags;
// 	m_constants[constant.name] = c;
// }
void Shader::LoadUniformBlock(const glslang::TProgram& program, VkShaderStageFlagBits stageFlag, int32_t i)
{
	auto uniformBlock = program.getUniformBlock(i);
	auto existUniformBlock = m_uniformBlocks.find(uniformBlock.name);
	if (existUniformBlock != m_uniformBlocks.end())
	{
		existUniformBlock->second.stageFlags |= stageFlag;
		return;
	}
	UniformBlock block;
	block.binding = uniformBlock.getBinding();
	block.bytes = ComputeBytes(uniformBlock.getType());
	block.stageFlags = stageFlag;
	auto& qualifier = uniformBlock.getType()->getQualifier();
	if (!STORAGE_QUALIFIER_TO_DESCRIPTOR_TYPE.contains(qualifier.storage))
	{
		throw std::runtime_error("Uniform block " + uniformBlock.name + " has unsupported storage " + std::to_string(qualifier.storage) + " for VkDescriptorType");
	}
	block.descriptorCount = uniformBlock.topLevelArrayStride > 0 ? uniformBlock.topLevelArrayStride : 1;
	block.descriptorType = STORAGE_QUALIFIER_TO_DESCRIPTOR_TYPE.at(qualifier.storage);
	block.isLayoutPushConstant = qualifier.layoutPushConstant;
	m_uniformBlocks[uniformBlock.name] = block;
}
void Shader::Initialize()
{
	if (!glslang::InitializeProcess())
	{
		throw std::runtime_error("Failed to initialize glslang process");
	}
}
void Shader::Uninitialize()
{
	glslang::FinalizeProcess();
}

bool Shader::LoadShaderForStage(const std::shared_ptr<File>& glslSource, const std::string& preamble, VkShaderStageFlagBits stageFlag)
{
	if (!STAGE_TO_LANGUAGE.contains(stageFlag))
	{
		throw std::runtime_error("Stage " + std::to_string(stageFlag) + " not supported");
	}
	if (m_shaderModules.contains(stageFlag))
	{
		throw std::runtime_error("Stage " + std::to_string(stageFlag) + " already loaded");
	}
	glslang::TProgram program;
	glslang::TShader shader(STAGE_TO_LANGUAGE.at(stageFlag));
	const TBuiltInResource* resource = GetDefaultResources();

	std::string shaderName = glslSource->GetNameWithoutExtension();
	std::string shaderSource = glslSource->ReadFile();
	const char* shaderNameCStr = shaderName.c_str();
	const char* shaderSourceCStr = shaderSource.c_str();
	auto vulkanVersion = glslang::EShTargetVulkan_1_3;
	shader.setStringsWithLengthsAndNames(&shaderSourceCStr, nullptr, &shaderNameCStr, 1);
	shader.setPreamble(preamble.c_str());
	shader.setEnvInput(glslang::EShSourceGlsl, shader.getStage(), glslang::EShClientVulkan, 110);
	shader.setEnvClient(glslang::EShClientVulkan, vulkanVersion);
	shader.setEnvTarget(glslang::EShTargetSpv, glslang::EShTargetSpv_1_0);

	EShMessages messages = static_cast<EShMessages>(EShMsgSpvRules | EShMsgVulkanRules | EShMsgDefault);
#if defined(_DEBUG)
	messages = static_cast<EShMessages>(messages | EShMsgDebugInfo);
#endif
	ShaderIncluder includer;
	std::string outputs;
	if (!shader.preprocess(resource, vulkanVersion, ENoProfile, false, false, messages, &outputs, includer))
	{
		throw std::runtime_error("GLSL Preprocessing failed for " + shaderName + ":\n" + shader.getInfoLog() + '\n' + shader.getInfoDebugLog());
	}
	if (!shader.parse(resource, vulkanVersion, true, messages, includer))
	{
		throw std::runtime_error("GLSL Parsing failed for " + shaderName + ":\n" + shader.getInfoLog() + '\n' + shader.getInfoDebugLog());
	}

	program.addShader(&shader);

	if (!program.link(messages) || !program.mapIO())
	{
		throw std::runtime_error("GLSL Linking failed for " + shaderName + ":\n" + program.getInfoLog() + '\n' + program.getInfoDebugLog());
	}

	program.buildReflection();

	for (int32_t i = 0; i < program.getNumLiveUniformBlocks(); i++)
	{
		LoadUniformBlock(program, stageFlag, i);
	}

	for (int32_t i = 0; i < program.getNumLiveUniformVariables(); i++)
	{
		LoadUniform(program, stageFlag, i);
	}
	for (int32_t i = 0; i < program.getNumLiveAttributes(); i++)
	{
		LoadAttribute(program, i);
	}

	glslang::SpvOptions spvOptions;
#if defined(_DEBUG)
	spvOptions.generateDebugInfo = true;
	spvOptions.disableOptimizer = true;
	spvOptions.optimizeSize = false;
#else
	spvOptions.generateDebugInfo = false;
	spvOptions.disableOptimizer = false;
	spvOptions.optimizeSize = true;
#endif

	spv::SpvBuildLogger logger;
	std::vector<uint32_t> spirv;
	glslang::GlslangToSpv(*program.getIntermediate(shader.getStage()), spirv, &logger, &spvOptions);

	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = spirv.size() * sizeof(uint32_t);
	createInfo.pCode = spirv.data();

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(m_device->vkDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create shader module!");
	}
	m_shaderModules[stageFlag] = shaderModule;
	return true;
}

uint32_t Shader::ComputeBytes(const glslang::TType* tType)
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
VkShaderModule Shader::GetShaderModuleStage(VkShaderStageFlagBits stageFlag)
{
	auto shaderModule = m_shaderModules.find(stageFlag);
	if (shaderModule != m_shaderModules.end())
	{
		return shaderModule->second;
	}
	return VK_NULL_HANDLE;
}
std::shared_ptr<DescriptorResource> Shader::CreateDescriptorResource()
{
	std::map<VkDescriptorType, uint32_t> descriptorPoolCount;
	DescriptorResource::CreateDescriptorPool(m_device);
	std::shared_ptr<DescriptorResource> result = std::make_shared<DescriptorResource>(m_device);
	uint32_t pushConstantOffset = 0;
	for (const auto& [uniformBlockName, uniformBlock] : m_uniformBlocks)
	{
		if (uniformBlock.isLayoutPushConstant)
		{
			VkPushConstantRange pushConstantRange{};
			pushConstantRange.stageFlags = uniformBlock.stageFlags;
			pushConstantRange.offset = pushConstantOffset;
			pushConstantRange.size = uniformBlock.bytes;
			pushConstantOffset += uniformBlock.bytes;
			result->vkPushConstantRanges.emplace_back(pushConstantRange);
		}
		else
		{
			result->nameToBinding[uniformBlockName] = uniformBlock.binding;
			VkDescriptorSetLayoutBinding layoutBinding{};
			layoutBinding.binding = uniformBlock.binding;
			layoutBinding.descriptorType = uniformBlock.descriptorType;
			layoutBinding.descriptorCount = uniformBlock.descriptorCount;
			layoutBinding.stageFlags = uniformBlock.stageFlags;
			layoutBinding.pImmutableSamplers = nullptr; // Optional
			auto descriptorPoolCountIt = descriptorPoolCount.find(uniformBlock.descriptorType);
			if (descriptorPoolCountIt != descriptorPoolCount.end())
			{
				descriptorPoolCountIt->second++;
			}
			else
			{
				descriptorPoolCount[uniformBlock.descriptorType] = 1;
			}
			result->vkDescriptorSetLayoutBindings.emplace_back(layoutBinding);
		}
	}
	for (const auto& [uniformName, uniform] : m_uniforms)
	{
		result->nameToBinding[uniformName] = uniform.binding;
		VkDescriptorSetLayoutBinding layoutBinding{};
		layoutBinding.binding = uniform.binding;
		layoutBinding.descriptorType = uniform.descriptorType;
		layoutBinding.descriptorCount = uniform.descriptorCount;
		layoutBinding.stageFlags = uniform.stageFlags;
		layoutBinding.pImmutableSamplers = nullptr; // Optional
		auto descriptorPoolCountIt = descriptorPoolCount.find(uniform.descriptorType);
		if (descriptorPoolCountIt != descriptorPoolCount.end())
		{
			descriptorPoolCountIt->second++;
		}
		else
		{
			descriptorPoolCount[uniform.descriptorType] = 1;
		}
		result->vkDescriptorSetLayoutBindings.emplace_back(layoutBinding);
	}

	uint32_t offset = 0;
	// 按照 layoutLocation 排序
	std::vector<std::pair<std::string, Attribute>> sortedAttributes{m_attributes.begin(), m_attributes.end()};
	std::sort(sortedAttributes.begin(), sortedAttributes.end(), [](const auto& a, const auto& b)
	{
		return a.second.layoutLocation < b.second.layoutLocation;
	});
	for (const auto& [attributeName, attribute] : sortedAttributes)
	{
		VkVertexInputAttributeDescription vertexInputAttributeDescription{};
		vertexInputAttributeDescription.location = attribute.layoutLocation;
		vertexInputAttributeDescription.binding = 0;
		if (!GL_DEFINE_TYPE_TO_FORMAT.contains(attribute.glDefineType))
		{
			throw std::runtime_error("Attribute " + attributeName + " has unsupported glDefineType " + std::to_string(attribute.glDefineType) + " for VkFormat");
		}
		vertexInputAttributeDescription.format = GL_DEFINE_TYPE_TO_FORMAT.at(attribute.glDefineType);
		vertexInputAttributeDescription.offset = offset;
		offset += attribute.bytes;
		result->vkVertexInputAttributeDescriptions.emplace_back(vertexInputAttributeDescription);
	}

	result->vkVertexInputBindingDescription.binding = 0;
	result->vkVertexInputBindingDescription.stride = offset;
	result->vkVertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	result->vkDescriptorSetLayout = result->CreateDescriptorSetLayout(result->vkDescriptorSetLayoutBindings);
	return result;
}
