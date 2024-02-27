#include "ApplicationEditor.hpp"

#include <memory>
#include <stdexcept>
#include "IEditor.hpp"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"
#include "Grid.hpp"
#include "Gizmos/TransformGizmo.hpp"
#include "Infrastructures/DataBinding.hpp"
#include "Infrastructures/SingletonOrganizer.hpp"
#include "Rendering/Application.hpp"

static bool cursorOff = false;
static bool moveMouse = true;
static float lastX = 0.0f;
static float lastY = 0.0f;
static GLFWcursorposfun lastCallback = nullptr;

static void SwitchCursor(Application& application, bool onEditorCamera)
{
	if (onEditorCamera == cursorOff)
	{
		return;
	}
	GLFWwindow* window = application.surface->glfwWindow;
	if (!cursorOff)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		lastCallback = glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xPos, double yPos)
		{
			auto applicationEditorDataBinding = DataBinding::Get<std::shared_ptr<ApplicationEditor>>("ApplicationEditor");
			std::shared_ptr<ApplicationEditor> applicationEditor = applicationEditorDataBinding->GetData();
			if (moveMouse)
			{
				lastX = (float)xPos;
				lastY = (float)yPos;
				moveMouse = false;
			}

			float offsetX = (float)xPos - lastX;
			float offsetY = lastY - (float)yPos;
			lastX = (float)xPos;
			lastY = (float)yPos;
			applicationEditor->editorCamera->ProcessMouseMovement(offsetX, offsetY);
		});
	}
	else
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		glfwSetCursorPosCallback(window, lastCallback);
		moveMouse = true;
	}
	cursorOff = !cursorOff;
}

static void CheckVkResult(VkResult err)
{
	if (err == 0)
		return;
	(void)fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
	if (err < 0)
		abort();
}

ApplicationEditor::ApplicationEditor()
{
	auto application = SingletonOrganizer::Get<Application>();
	m_device = application->device;
	auto subpass = std::make_shared<Subpass>(m_device, false);
	subpass->BeginSubpassAttachments();
	subpass->AddColorAttachment("editor", Swapchain::COLOR_FORMAT, VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
	subpass->EndSubpassAttachments();
	renderPass = std::make_shared<RenderPass>(application->device, subpass);
	CreateFramebuffer();

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
	init_info.DescriptorPool = DescriptorResource::vkDescriptorPool;
	init_info.Subpass = 0;
	init_info.MinImageCount = static_cast<uint32_t>(vkImageViews.size());
	init_info.ImageCount = static_cast<uint32_t>(application->swapchain->vkImages.size());
	init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
	init_info.Allocator = nullptr;
	init_info.CheckVkResultFn = CheckVkResult;
	init_info.RenderPass = renderPass->vkRenderPass;
	ImGui_ImplVulkan_Init(&init_info);
	editorCamera = application->editorCamera;
	settings = application->settings;
	grid = std::make_shared<Grid>(m_device, application->commandResource, renderPass);
	transformGizmo = std::make_shared<TransformGizmo>(application->modelGameObject, m_device, application->commandResource, application->mainPipeline, application->mainPipeline->descriptorResource,
	                                                  renderPass);
	
}

ApplicationEditor::~ApplicationEditor()
{
	Cleanup();
}

void ApplicationEditor::CreateFramebuffer()
{
	auto application = SingletonOrganizer::Get<Application>();
	uint32_t imageCount = static_cast<uint32_t>(application->swapchain->vkImages.size());
	// Create The Image Views
	{
		vkImageViews.resize(imageCount);
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
		for (uint32_t i = 0; i < imageCount; i++)
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
		vkFramebuffers.resize(imageCount);
		VkImageView attachment[1];
		VkFramebufferCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		info.renderPass = renderPass->vkRenderPass;
		info.attachmentCount = 1;
		info.pAttachments = attachment;
		info.width = application->swapchain->vkExtent2D.width;
		info.height = application->swapchain->vkExtent2D.height;
		info.layers = 1;
		for (uint32_t i = 0; i < imageCount; i++)
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
	transformGizmo->Cleanup();
	grid->Cleanup();
	CleanupFramebuffers();

	renderPass->Cleanup();
	m_cleaned = true;
}

void ApplicationEditor::CleanupFramebuffers()
{
	for (auto framebuffer : vkFramebuffers)
	{
		vkDestroyFramebuffer(m_device->vkDevice, framebuffer, nullptr);
	}
	for (auto imageView : vkImageViews)
	{
		vkDestroyImageView(m_device->vkDevice, imageView, nullptr);
	}
}

// void ApplicationEditor::NewFrame()
// {
// 	// Start the Dear ImGui frame
// 	ImGui_ImplVulkan_NewFrame();
// 	ImGui_ImplGlfw_NewFrame();
// 	ImGui::NewFrame();
// }
// void ApplicationEditor::DrawGizmos()
// {
// 	gizmoEditor->DrawFrame();
// }

void ApplicationEditor::DrawFrame(Application& application, VkCommandBuffer currentCommandBuffer, uint32_t currentFrame, std::shared_ptr<SyncObjects> syncObjects, uint32_t imageIndex)
{
	ImGuiIO& io = ImGui::GetIO();

	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	if (show_demo_window)
	{
		ImGui::ShowDemoWindow(&show_demo_window);
	}

	for (auto it = IEditor::registeredEditors.cbegin(); it != IEditor::registeredEditors.cend(); ++it)
	{
		it->get()->DrawFrame();
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
	// const bool main_is_minimized = (main_draw_data->DisplaySize.x <= 0.0f || main_draw_data->DisplaySize.y <= 0.0f);
	application.clearColor.r = clear_color.x * clear_color.w;
	application.clearColor.g = clear_color.y * clear_color.w;
	application.clearColor.b = clear_color.z * clear_color.w;
	application.clearColor.a = clear_color.w;
	// if (!main_is_minimized)
	// {
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0; // Optional
	beginInfo.pInheritanceInfo = nullptr; // Optional

	if (vkBeginCommandBuffer(currentCommandBuffer, &beginInfo) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to begin recording command buffer!");
	}
	{
		VkRenderPassBeginInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		info.renderPass = renderPass->vkRenderPass;
		info.framebuffer = vkFramebuffers[imageIndex];
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
	if (vkEndCommandBuffer(currentCommandBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to record command buffer!");
	}
	// }

	// Update and Render additional Platform Windows
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}

	// Present Main Platform Window
	// if (!main_is_minimized)
	// {
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = {syncObjects->gameRenderFinishedSemaphores[currentFrame]};
	VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &currentCommandBuffer;
	VkSemaphore signalSemaphores[] = {syncObjects->renderFinishedSemaphores[currentFrame]};
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;
	if (vkQueueSubmit(m_device->graphicsQueue, 1, &submitInfo, syncObjects->inFlightFences[currentFrame]) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to submit editor draw command buffer!");
	}
	// }

	bool isRightButtonPressed = glfwGetMouseButton(application.surface->glfwWindow, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
	if (isRightButtonPressed)
	{
		editorCamera->UpdatePosition(application.deltaTime, application.surface->glfwWindow);
		application.settings->UpdateEditorCamera(editorCamera);
	}
	SwitchCursor(application, isRightButtonPressed);
}

void ApplicationEditor::CleanupWhenRecreateSwapchain()
{
	CleanupFramebuffers();
}
