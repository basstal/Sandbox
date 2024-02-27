#pragma once
#include <map>
#include <memory>
#include <string>
#include <glslang/Public/ShaderLang.h>
#include <vulkan/vulkan_core.h>

class DescriptorResource;
class Device;
class File;

struct Uniform
{
	int32_t binding;
	uint32_t bytes;
	int32_t glDefineType;
	int32_t offset;
	uint32_t descriptorCount;
	VkDescriptorType descriptorType;
	VkShaderStageFlags stageFlags;
};

struct UniformBlock
{
	int32_t binding;
	uint32_t bytes;
	VkShaderStageFlags stageFlags;
	uint32_t descriptorCount;
	VkDescriptorType descriptorType;
	bool isLayoutPushConstant;
	std::map<std::string, Uniform> uniforms;
};

struct Attribute
{
	uint32_t layoutSet;
	uint32_t layoutLocation;
	uint32_t bytes;
	int32_t glDefineType;
};

class Shader
{
private:
	// struct Constant
	// {
	// 	uint32_t size;
	// 	VkShaderStageFlags stageFlags;
	// };


	std::map<std::string, Uniform> m_uniforms;
	std::map<std::string, UniformBlock> m_uniformBlocks;
	std::map<std::string, Attribute> m_attributes;
	// std::map<std::string, Constant> m_constants;
	std::shared_ptr<Device> m_device;
	std::map<VkShaderStageFlagBits, VkShaderModule> m_shaderModules;
	void LoadUniform(const glslang::TProgram& program, VkShaderStageFlagBits stageFlag, int32_t i);
	void LoadAttribute(const glslang::TProgram& program, int32_t i);
	void LoadUniformBlock(const glslang::TProgram& program, VkShaderStageFlagBits stageFlag, int32_t i);
	uint32_t ComputeBytes(const glslang::TType* tType);

public:
	static void Initialize();
	static void Uninitialize();
	Shader(const std::shared_ptr<Device>& device);
	~Shader();
	bool LoadShaderForStage(const std::shared_ptr<File>& glslSource, const std::string& preamble, VkShaderStageFlagBits stageFlag);
	VkShaderModule GetShaderModuleStage(VkShaderStageFlagBits stageFlag);
	std::shared_ptr<DescriptorResource> CreateDescriptorResource();
};
