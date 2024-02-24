#include "CubeMap.hpp"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "Components/Buffer.hpp"
#include "Components/Pipeline.hpp"
#include "Infrastructures/FileSystemBase.hpp"

const glm::mat4 EquirectangularCaptureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
const glm::mat4 EquirectangularCaptureViews[] =
{
	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))
};

CubeMap::CubeMap(const std::shared_ptr<Device>& device, const std::shared_ptr<Image>& image, const std::shared_ptr<CommandResource>& commandResource, uint32_t inSize,
                 const std::shared_ptr<Pipeline>& pipeline, const std::shared_ptr<RenderPass>& renderPass, const std::shared_ptr<DescriptorResource>& descriptorResource)
{
	m_device = device;
	size = inSize;
	irradianceMap = std::make_shared<RenderTexture>(device, commandResource, false, true, image->Width(), image->Height(), image->MipLevels());
	irradianceMap->AssignImageData(image);

	for (uint32_t i = 0; i < 6; ++i)
	{
		auto vkFormat = irradianceMap->vkFormat;
		m_device->CreateImage(size, size, 1, VK_SAMPLE_COUNT_1_BIT, vkFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		                      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vkImageCubeMap[i], vkDeviceMemoriesCubeMap[i], false);
		VkImageViewCreateInfo viewInfo = {};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = vkImageCubeMap[i];
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = vkFormat;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;
		vkCreateImageView(device->vkDevice, &viewInfo, nullptr, &vkImageViewCubeMap[i]);
	}

	renderTexture = std::make_shared<RenderTexture>(device, commandResource, true, true, inSize, inSize, 1);
	// renderTexture->TransitionImageLayout(renderTexture->vkFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, 1);
	vkRenderPass = renderPass->CreateCubeMapRenderPass();

	VkDescriptorSetLayoutBinding mvpLayoutBinding;
	mvpLayoutBinding.binding = 0;
	mvpLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	mvpLayoutBinding.descriptorCount = 1;
	mvpLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	mvpLayoutBinding.pImmutableSamplers = nullptr; // Optional


	VkDescriptorSetLayoutBinding irradianceMapLayoutBinding;
	irradianceMapLayoutBinding.binding = 1;
	irradianceMapLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	irradianceMapLayoutBinding.descriptorCount = 1;
	irradianceMapLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	irradianceMapLayoutBinding.pImmutableSamplers = nullptr; // Optional
	vkDescriptorSetLayout = descriptorResource->CreateDescriptorSetLayout({mvpLayoutBinding, irradianceMapLayoutBinding});
	vkPipelineLayout = pipeline->CreatePipelineLayout(vkDescriptorSetLayout);

	std::filesystem::path binariesDir = FileSystemBase::getBinariesDir();
	auto vertexShader = FileSystemBase::readFile((binariesDir / "Shaders/EquirectangularMap_vert.spv").string());
	auto fragmentShader = FileSystemBase::readFile((binariesDir / "Shaders/EquirectangularMap_frag.spv").string());
	VkShaderModule vertShaderModule = Pipeline::CreateShaderModule(device, vertexShader);
	VkShaderModule fragShaderModule = Pipeline::CreateShaderModule(device, fragmentShader);

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
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkPipelineDepthStencilStateCreateInfo depthStencil{};
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable = VK_TRUE;
	depthStencil.depthWriteEnable = VK_TRUE;
	depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.minDepthBounds = 0.0f; // Optional
	depthStencil.maxDepthBounds = 1.0f; // Optional
	depthStencil.stencilTestEnable = VK_FALSE;
	depthStencil.front = {}; // Optional
	depthStencil.back = {}; // Optional
	vkPipeline = pipeline->CreatePipeline(vertShaderModule, fragShaderModule, false, vertexInputInfo, inputAssembly, depthStencil, vkPipelineLayout, vkRenderPass, false);

	vkDestroyShaderModule(m_device->vkDevice, fragShaderModule, nullptr);
	vkDestroyShaderModule(m_device->vkDevice, vertShaderModule, nullptr);
}
CubeMap::~CubeMap()
{
	Cleanup();
}

void CubeMap::Cleanup()
{
	if (m_cleaned)
	{
		return;
	}
	if (renderTexture != nullptr)
	{
		renderTexture->Cleanup();
	}
	if (irradianceMap != nullptr)
	{
		irradianceMap->Cleanup();
	}
	vkDestroyDescriptorSetLayout(m_device->vkDevice, vkDescriptorSetLayout, nullptr);
	vkDestroyPipeline(m_device->vkDevice, vkPipeline, nullptr);
	vkDestroyPipelineLayout(m_device->vkDevice, vkPipelineLayout, nullptr);
	vkDestroyRenderPass(m_device->vkDevice, vkRenderPass, nullptr);
	for (uint32_t i = 0; i < 6; ++i)
	{
		vkDestroyImageView(m_device->vkDevice, vkImageViewCubeMap[i], nullptr);
	}
	if (vertexBuffer != nullptr)
	{
		vertexBuffer->Cleanup();
	}
	m_cleaned = true;
}

void CubeMap::CreateDescriptorSet(const std::shared_ptr<UniformBuffers>& uniformBuffers, const std::shared_ptr<DescriptorResource>& descriptorResource)
{
	std::vector<VkDescriptorSetLayout> layouts(1, vkDescriptorSetLayout);
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorResource->vkDescriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(1);
	allocInfo.pSetLayouts = layouts.data();
	// vkDescriptorSets.resize(m_device->MAX_FRAMES_IN_FLIGHT);
	if (vkAllocateDescriptorSets(m_device->vkDevice, &allocInfo, &vkDescriptorSet) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate descriptor sets!");
	}
	VkDescriptorBufferInfo mvpBufferInfo;
	mvpBufferInfo.buffer = uniformBuffers->mvpObjectBuffers[0]->vkBuffer;
	mvpBufferInfo.offset = 0;
	mvpBufferInfo.range = sizeof(MVPObject);


	VkWriteDescriptorSet mvpWrite{};
	mvpWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	mvpWrite.dstSet = vkDescriptorSet;
	mvpWrite.dstBinding = 0;
	mvpWrite.dstArrayElement = 0;
	mvpWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	mvpWrite.descriptorCount = 1;
	mvpWrite.pBufferInfo = &mvpBufferInfo;
	mvpWrite.pImageInfo = nullptr; // Optional
	mvpWrite.pTexelBufferView = nullptr; // Optional


	VkDescriptorImageInfo irradianceImageInfo;
	irradianceImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	irradianceImageInfo.imageView = irradianceMap->vkImageView;
	irradianceImageInfo.sampler = irradianceMap->vkSampler;

	VkWriteDescriptorSet irradianceMapWrite{};
	irradianceMapWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	irradianceMapWrite.dstSet = vkDescriptorSet;
	irradianceMapWrite.dstBinding = 1;
	irradianceMapWrite.dstArrayElement = 0;
	irradianceMapWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	irradianceMapWrite.descriptorCount = 1;
	irradianceMapWrite.pImageInfo = &irradianceImageInfo;

	std::array descriptorWrites{mvpWrite, irradianceMapWrite};
	vkUpdateDescriptorSets(m_device->vkDevice, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
}

VkVertexInputBindingDescription CubeMap::GetBindingDescription()
{
	VkVertexInputBindingDescription bindingDescription;
	bindingDescription.binding = 0;
	bindingDescription.stride = 8 * sizeof(float);
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	return bindingDescription;
}

std::array<VkVertexInputAttributeDescription, 3> CubeMap::GetAttributeDescriptions()
{
	std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

	attributeDescriptions[0].binding = 0;
	attributeDescriptions[0].location = 0;
	attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[0].offset = 0;

	attributeDescriptions[1].binding = 0;
	attributeDescriptions[1].location = 1;
	attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[1].offset = 3 * sizeof(float);

	attributeDescriptions[2].binding = 0;
	attributeDescriptions[2].location = 2;
	attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
	attributeDescriptions[2].offset = 6 * sizeof(float);
	return attributeDescriptions;
}




void CubeMap::RenderCube(const std::shared_ptr<CommandResource>& commandResource, const std::shared_ptr<UniformBuffers>& uniformBuffers, const std::shared_ptr<DescriptorResource>& descriptorResource)
{
	if (vertexBuffer == nullptr)
	{
		const float vertices[] = {
			// back face
			-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
			1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
			1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
			-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			-1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, // top-left
			// front face
			-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
			1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, // bottom-right
			1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right
			1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right
			-1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, // top-left
			-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
			// left face
			-1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
			-1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-left
			-1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
			// right face
			1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-left
			1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
			1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-right         
			1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
			1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-left
			1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-left     
			// bottom face
			-1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
			1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, // top-left
			1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom-left
			1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom-left
			-1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
			// top face
			-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
			1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
			1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // top-right     
			1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
			-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
			-1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f // bottom-left        
		};
		CreateDescriptorSet(uniformBuffers, descriptorResource);

		VkDeviceSize bufferSize = sizeof(vertices);
		Buffer vertexStagingBuffer(m_device, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		void* data;
		vkMapMemory(m_device->vkDevice, vertexStagingBuffer.vkDeviceMemory, 0, bufferSize, 0, &data);
		memcpy(data, vertices, (size_t)bufferSize);
		vkUnmapMemory(m_device->vkDevice, vertexStagingBuffer.vkDeviceMemory);

		vertexBuffer = std::make_shared<Buffer>(m_device, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		CopyBuffer(vertexStagingBuffer.vkBuffer, vertexBuffer->vkBuffer, bufferSize, commandResource);

		for (uint32_t i = 0; i < vkFramebuffers.size(); ++i)
		{
			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = vkRenderPass;
			framebufferInfo.attachmentCount = 1;
			framebufferInfo.pAttachments = &vkImageViewCubeMap[i];
			framebufferInfo.width = size;
			framebufferInfo.height = size;
			framebufferInfo.layers = 1;

			if (vkCreateFramebuffer(m_device->vkDevice, &framebufferInfo, nullptr, &vkFramebuffers[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create framebuffer!");
			}
		}
	}

	VkCommandBuffer commandBuffer = commandResource->BeginSingleTimeCommands();

	// 设置渲染目标的视口
	VkViewport viewport = {};
	viewport.width = static_cast<float>(size);
	viewport.height = static_cast<float>(size);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	VkRect2D scissor;
	scissor.offset = {0, 0};
	scissor.extent = {size, size};
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	// 对每个立方图面进行迭代
	for (size_t i = 0; i < 6; ++i)
	{
		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = vkRenderPass;
		renderPassInfo.framebuffer = vkFramebuffers[i];
		renderPassInfo.renderArea.offset = {0, 0};
		renderPassInfo.renderArea.extent = {size, size};
		std::array<VkClearValue, 1> clearValues{};
		clearValues[0].color = {{1.0f, 1.0f, 1.0f, 0.0f}};
		// clearValues[1].depthStencil = {1.0f, 0};

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();
		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		// 更新视图矩阵 Uniform
		uniformBuffers->UpdateMVP(0, EquirectangularCaptureViews[i], glm::mat4(1.0f), EquirectangularCaptureProjection);


		// 绑定管线和顶点缓冲区等
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipeline);
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipelineLayout, 0, 1, &vkDescriptorSet, 0, nullptr);
		VkBuffer vertexBuffers[] = {vertexBuffer->vkBuffer};
		VkDeviceSize offsets[] = {0};
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

		// 如果使用索引缓冲区，也绑定它
		// vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT16);

		// 绘制立方体
		vkCmdDraw(commandBuffer, 36, 1, 0, 0);
		vkCmdEndRenderPass(commandBuffer);
	}

	commandResource->EndSingleTimeCommands(commandBuffer);
}


void CubeMap::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize deviceSize, const std::shared_ptr<CommandResource>& commandResource)
{
	VkCommandBuffer commandBuffer = commandResource->BeginSingleTimeCommands();
	VkBufferCopy copyRegion;
	copyRegion.srcOffset = 0; // Optional
	copyRegion.dstOffset = 0; // Optional
	copyRegion.size = deviceSize;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
	commandResource->EndSingleTimeCommands(commandBuffer);
}
