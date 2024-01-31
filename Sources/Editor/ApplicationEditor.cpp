#include "ApplicationEditor.hpp"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"
#include "Rendering/Application.hpp"

static void check_vk_result(VkResult err)
{
	if (err == 0)
		return;
	(void)fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
	if (err < 0)
		abort();
}

ApplicationEditor::ApplicationEditor(const std::unique_ptr<Application>& application)
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForVulkan(application->surface->glfwWindow, true);
	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = application->vkInstance;
	init_info.PhysicalDevice = application->device->vkPhysicalDevice;
	init_info.Device = application->device->vkDevice;
	init_info.QueueFamily = *application->device->queueFamilies.graphicsFamily;
	init_info.Queue = application->device->graphicsQueue;
	init_info.PipelineCache = VK_NULL_HANDLE; // TODO: PipelineCache
	init_info.DescriptorPool = application->descriptorResource->vkDescriptorPool;
	init_info.Subpass = 0;
	init_info.MinImageCount = 2;
	init_info.ImageCount = static_cast<uint32_t>(application->swapchain->vkImages.size());
	init_info.MSAASamples = application->device->msaaSamples;
	init_info.Allocator = nullptr;
	init_info.CheckVkResultFn = check_vk_result;
	ImGui_ImplVulkan_Init(&init_info, application->renderPass->vkRenderPass);
}

ApplicationEditor::~ApplicationEditor()
{
}

void ApplicationEditor::DrawFrame(Application& application, VkCommandBuffer currentCommandBuffer)
{
	ImGuiIO& io = ImGui::GetIO();

	// Start the Dear ImGui frame
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	if (show_demo_window)
	{
		ImGui::ShowDemoWindow(&show_demo_window);
	}

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
	{
		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("Hello, world!"); // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text."); // Display some text (you can use a format strings too)
		ImGui::Checkbox("Demo Window", &show_demo_window); // Edit bools storing our window open/close state
		ImGui::Checkbox("Another Window", &show_another_window);

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f); // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

		if (ImGui::Button("Button")) // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
		ImGui::End();
	}

	// 3. Show another simple window.
	if (show_another_window)
	{
		ImGui::Begin("Another Window", &show_another_window); // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Close Me"))
			show_another_window = false;
		ImGui::End();
	}

	// Rendering
	ImGui::Render();
	ImDrawData* main_draw_data = ImGui::GetDrawData();
	const bool main_is_minimized = (main_draw_data->DisplaySize.x <= 0.0f || main_draw_data->DisplaySize.y <= 0.0f);
	application.clearColor.r = clear_color.x * clear_color.w;
	application.clearColor.g = clear_color.y * clear_color.w;
	application.clearColor.b = clear_color.z * clear_color.w;
	application.clearColor.a = clear_color.w;
	if (!main_is_minimized)
	{
		// Record dear imgui primitives into command buffer
		ImGui_ImplVulkan_RenderDrawData(main_draw_data, currentCommandBuffer);
	}

	// Update and Render additional Platform Windows
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}
