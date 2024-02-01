#pragma once
#include "backends/imgui_impl_vulkan.h"
#include "Rendering/Application.hpp"

class ApplicationEditor
{
private:
	bool m_cleaned = false;
	std::shared_ptr<Device> m_device;

public:
	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	static const uint32_t IMAGE_COUNT = 2;

	std::shared_ptr<RenderPass> renderPass;
	std::vector<VkImageView> vkImageViews;
	std::vector<VkFramebuffer> vkFramebuffers;

	ApplicationEditor(const std::unique_ptr<Application>& application);
	~ApplicationEditor();
	void CreateFramebuffer(const std::unique_ptr<Application>& application);
	void Cleanup();
	void DrawFrame(Application& application, VkCommandBuffer currentCommandBuffer, uint32_t currentFrame);
};
