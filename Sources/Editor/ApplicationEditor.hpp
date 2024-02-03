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

	std::shared_ptr<RenderPass> renderPass;
	std::vector<VkImageView> vkImageViews;
	std::vector<VkFramebuffer> vkFramebuffers;

	ApplicationEditor(const std::unique_ptr<Application>& application);
	~ApplicationEditor();
	void CreateFramebuffer(const std::unique_ptr<Application>& application);
	void Cleanup();
	void CleanupFramebuffers();
	void DrawFrame(Application& application, VkCommandBuffer currentCommandBuffer, uint32_t currentFrame, std::shared_ptr<SyncObjects> syncObjects, uint32_t imageIndex);
	void CleanupWhenRecreateSwapchain();
};
