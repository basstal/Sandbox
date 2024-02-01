#include "ApplicationEditor.hpp"

#include <stdexcept>

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
	m_device = application->device;
	renderPass = std::make_shared<RenderPass>(application->device, application->swapchain, RenderPassType::EDITOR_RENDER_PASS);
	CreateFramebuffer(application);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking
	// io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows

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
	init_info.MinImageCount = IMAGE_COUNT;
	init_info.ImageCount = static_cast<uint32_t>(application->swapchain->vkImages.size());
	init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
	init_info.Allocator = nullptr;
	init_info.CheckVkResultFn = check_vk_result;
	ImGui_ImplVulkan_Init(&init_info, renderPass->vkRenderPass);
}

ApplicationEditor::~ApplicationEditor()
{
	Cleanup();
}

void ApplicationEditor::CreateFramebuffer(const std::unique_ptr<Application>& application)
{
	// // Create Image
	// {
	// 	auto vkExtent2D = application->swapchain->vkExtent2D;
	// 	std::shared_ptr<Device> device = application->device;
	// 	for (uint32_t i = 0; i < IMAGE_COUNT; i++)
	// 	{
	// 		device->CreateImage(vkExtent2D.width, vkExtent2D.height, 1, VK_SAMPLE_COUNT_1_BIT, Swapchain::COLOR_FORMAT,
	// 		                    VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
	// 		                    vkImages[i], m_vkDepthImageMemory);
	// 	}
	// }
	// Create The Image Views
	{
		vkImageViews.resize(IMAGE_COUNT);
		VkImageViewCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		info.format = Swapchain::COLOR_FORMAT;
		info.components.r = VK_COMPONENT_SWIZZLE_R;
		info.components.g = VK_COMPONENT_SWIZZLE_G;
		info.components.b = VK_COMPONENT_SWIZZLE_B;
		info.components.a = VK_COMPONENT_SWIZZLE_A;
		VkImageSubresourceRange image_range = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
		info.subresourceRange = image_range;
		for (uint32_t i = 0; i < IMAGE_COUNT; i++)
		{
			info.image = application->swapchain->vkImages[i];
			if (vkCreateImageView(application->device->vkDevice, &info, nullptr, &vkImageViews[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create framebuffer!");
			}
		}
	}

	// Create Framebuffer
	{
		vkFramebuffers.resize(IMAGE_COUNT);
		VkImageView attachment[1];
		VkFramebufferCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		info.renderPass = renderPass->vkRenderPass;
		info.attachmentCount = 1;
		info.pAttachments = attachment;
		info.width = application->swapchain->vkExtent2D.width;
		info.height = application->swapchain->vkExtent2D.height;
		info.layers = 1;
		for (uint32_t i = 0; i < IMAGE_COUNT; i++)
		{
			attachment[0] = vkImageViews[i];
			if (vkCreateFramebuffer(application->device->vkDevice, &info, nullptr, &vkFramebuffers[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create framebuffer!");
			}
		}
	}
}

void ApplicationEditor::Cleanup()
{
	if (m_cleaned)
	{
		return;
	}
	for (auto framebuffer : vkFramebuffers)
	{
		vkDestroyFramebuffer(m_device->vkDevice, framebuffer, nullptr);
	}
	for (auto imageView : vkImageViews)
	{
		vkDestroyImageView(m_device->vkDevice, imageView, nullptr);
	}

	renderPass->Cleanup();
	m_cleaned = true;
}

void ApplicationEditor::DrawFrame(Application& application, VkCommandBuffer currentCommandBuffer, uint32_t currentFrame)
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
		{
			VkRenderPassBeginInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			info.renderPass = renderPass->vkRenderPass;
			info.framebuffer = vkFramebuffers[currentFrame];
			info.renderArea.extent = application.swapchain->vkExtent2D;
			// std::array<VkClearValue, 2> clearValues{};
			// clearValues[0].color = {{application.clearColor.r, application.clearColor.g, application.clearColor.b, application.clearColor.a}};
			// clearValues[1].depthStencil = {1.0f, 0};

			// info.clearValueCount = static_cast<uint32_t>(clearValues.size());
			// info.pClearValues = clearValues.data();
			vkCmdBeginRenderPass(currentCommandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
		}
		// Record dear imgui primitives into command buffer
		ImGui_ImplVulkan_RenderDrawData(main_draw_data, currentCommandBuffer);
		// Submit command buffer
		vkCmdEndRenderPass(currentCommandBuffer);
	}

	// Update and Render additional Platform Windows
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}
