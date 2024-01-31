#pragma once
#include "backends/imgui_impl_glfw.h"
#include "Rendering/Application.hpp"

class ApplicationEditor
{
public:
	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	ApplicationEditor(const std::unique_ptr<Application>& application);
	~ApplicationEditor();
	void DrawFrame(Application& application, VkCommandBuffer currentCommandBuffer);
};
