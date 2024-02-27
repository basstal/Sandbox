#pragma once

#include "backends/imgui_impl_vulkan.h"
#include "Grid.hpp"
#include "Gizmos/TransformGizmo.hpp"
#include "Rendering/Application.hpp"

class GizmoEditor;

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
	std::shared_ptr<Camera> editorCamera;
	std::shared_ptr<Settings> settings;
	// std::shared_ptr<GizmoEditor> gizmoEditor;
	std::shared_ptr<Grid> grid;
	std::shared_ptr<TransformGizmo> transformGizmo;


	ApplicationEditor();
	~ApplicationEditor();
	void CreateFramebuffer();
	void Cleanup();
	void CleanupFramebuffers();
	// void NewFrame();
	// void DrawGizmos(Application& application, VkCommandBuffer currentCommandBuffer, uint32_t imageIndex);
	// void RenderDrawData(Application& application, VkCommandBuffer currentCommandBuffer, uint32_t imageIndex);
	void DrawFrame(Application& application, VkCommandBuffer currentCommandBuffer, uint32_t currentFrame, std::shared_ptr<SyncObjects> syncObjects, uint32_t imageIndex);
	void CleanupWhenRecreateSwapchain();
};
