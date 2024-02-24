#include "Grid.hpp"

#include "Gizmos/SimpleVertex.hpp"
#include "Infrastructures/FileSystemBase.hpp"
#include "Rendering/Components/Buffer.hpp"
#include "Rendering/Components/CommandResource.hpp"
#include "Rendering/Components/Pipeline.hpp"


std::vector<SimpleVertex> Grid::GetLineListProperties()
{
	std::vector<SimpleVertex> lineListProperty;
	auto halfSize = glm::vec2(size.x / 2, size.y / 2);
	auto gray = glm::vec3(0.5f, 0.5f, 0.5f);
	auto deeperGray = glm::vec3(0.3f, 0.3f, 0.3f);
	auto red = glm::vec3(1.0f, 0.0f, 0.0f);
	auto green = glm::vec3(0.0f, 1.0f, 0.0f);
	// 使中央的线段 x 方向用红色，y 方向用绿色
	for (float i = -halfSize.x; i <= halfSize.x; i += unitSize)
	{
		SimpleVertex property;
		property.position = glm::vec3(i, -halfSize.y, 0.0f);
		if (i == 0.0f)
		{
			property.color = green;
		}
		else if (static_cast<int>(i) % static_cast<int>(gridSize) == 0)
		{
			property.color = deeperGray;
		}
		else
		{
			property.color = gray;
		}
		lineListProperty.push_back(property);
		property.position = glm::vec3(i, halfSize.y, 0.0f);
		lineListProperty.push_back(property);
	}
	for (float i = -halfSize.y; i <= halfSize.y; i += unitSize)
	{
		SimpleVertex property;
		property.position = glm::vec3(-halfSize.x, i, 0.0f);
		if (i == 0.0f)
		{
			property.color = red;
		}
		else if (static_cast<int>(i) % static_cast<int>(gridSize) == 0)
		{
			property.color = deeperGray;
		}
		else
		{
			property.color = gray;
		}
		lineListProperty.push_back(property);
		property.position = glm::vec3(halfSize.x, i, 0.0f);
		lineListProperty.push_back(property);
	}
	return lineListProperty;
}

Grid::Grid(const std::shared_ptr<Device>& device, const std::shared_ptr<CommandResource>& commandResource, const std::shared_ptr<Pipeline>& pipeline)
{
	m_device = device;

	PrepareDrawData(device, commandResource, pipeline);
}

Grid::~Grid()
{
	Cleanup();
}
void Grid::Cleanup()
{
	if (m_cleaned)
	{
		return;
	}
	if (vkPipeline != nullptr)
	{
		vkDestroyPipeline(m_device->vkDevice, vkPipeline, nullptr);
	}
	if (buffer != nullptr)
	{
		buffer->Cleanup();
	}
	m_cleaned = true;
}



void Grid::PrepareDrawData(const std::shared_ptr<Device>& device, const std::shared_ptr<CommandResource>& commandResource, const std::shared_ptr<Pipeline>& pipeline)
{
	lineListProperties = GetLineListProperties();
	auto vkDevice = device->vkDevice;
	VkDeviceSize bufferSize = sizeof(SimpleVertex) * lineListProperties.size();

	buffer = std::make_shared<Buffer>(device, bufferSize,
	                                  VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
	                                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	Buffer bufferStaging(device, bufferSize,
	                     VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
	                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	// 将顶点数据复制到暂存缓冲
	void* data;
	vkMapMemory(vkDevice, bufferStaging.vkDeviceMemory, 0, bufferSize, 0, &data);
	memcpy(data, lineListProperties.data(), bufferSize);
	vkUnmapMemory(vkDevice, bufferStaging.vkDeviceMemory);

	// 复制暂存缓冲到顶点缓冲
	VkCommandBuffer commandBuffer = commandResource->BeginSingleTimeCommands();
	VkBufferCopy copyRegion;
	copyRegion.srcOffset = 0; // Optional
	copyRegion.dstOffset = 0; // Optional
	copyRegion.size = bufferSize;
	vkCmdCopyBuffer(commandBuffer, bufferStaging.vkBuffer, buffer->vkBuffer, 1, &copyRegion);
	commandResource->EndSingleTimeCommands(commandBuffer);

	std::filesystem::path binariesDir = FileSystemBase::getBinariesDir();
	auto nonSolidVertex = FileSystemBase::readFile((binariesDir / "Shaders/FillModeNonSolidGrid_vert.spv").string());
	auto nonSolidFrag = FileSystemBase::readFile((binariesDir / "Shaders/FillModeNonSolidGrid_frag.spv").string());
	VkShaderModule vertShaderModule = pipeline->CreateShaderModule(m_device, nonSolidVertex);
	VkShaderModule fragShaderModule = pipeline->CreateShaderModule(m_device, nonSolidFrag);

	// 创建管线
	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	auto bindingDescription = GetBindingDescription();
	auto attributeDescriptions = GetAttributeDescriptions();

	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;
	vkPipeline = pipeline->CreatePipeline(vertShaderModule, fragShaderModule, true, vertexInputInfo, inputAssembly);
}

void Grid::Draw(const std::shared_ptr<Device>& device, const VkCommandBuffer& currentCommandBuffer, const std::shared_ptr<Pipeline>& pipeline,
                const std::shared_ptr<DescriptorResource>& descriptorResource, uint32_t currentFrame)
{
	vkCmdBindPipeline(currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipeline);


	VkBuffer vertexBuffers[] = {buffer->vkBuffer};
	VkDeviceSize offsets[] = {0};
	vkCmdBindVertexBuffers(currentCommandBuffer, 0, 1, vertexBuffers, offsets);
	vkCmdBindDescriptorSets(currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->vkPipelineLayout, 0, 1, &descriptorResource->vkDescriptorSets[currentFrame], 0, nullptr);
	vkCmdDraw(currentCommandBuffer, static_cast<uint32_t>(lineListProperties.size()), 1, 0, 0);
}
