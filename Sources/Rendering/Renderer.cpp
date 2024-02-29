#include "Renderer.hpp"

#include <filesystem>
#include <memory>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>

#include "RendererSettings.hpp"
#include "GameCore/Model.hpp"
#include "Infrastructures/FileSystem/FileSystemBase.hpp"
#include "Components/RenderPass.hpp"
#include "Base/Device.hpp"
#include "Buffers/IndexBuffer.hpp"
#include "Buffers/UniformBuffer.hpp"
#include "Buffers/VertexBuffer.hpp"
#include "Components/CommandResource.hpp"
#include "Components/DescriptorResource.hpp"
#include "Components/Pipeline.hpp"
#include "Components/Subpass.hpp"
#include "Components/Swapchain.hpp"
#include "Editor/ApplicationEditor.hpp"
#include "GameCore/GameObject.hpp"
#include "GameCore/Transform.hpp"
#include "Infrastructures/DataBinding.hpp"
#include "Infrastructures/SingletonOrganizer.hpp"
#include "Infrastructures/FileSystem/File.hpp"
#include "Objects/Framebuffer.hpp"
#include "Objects/Shader.hpp"
#include "Camera.hpp"
#include "Buffers/UniformCombinedImage.hpp"
#include "Rendering/Light.hpp"
#include "Rendering/Base/Surface.hpp"


std::vector<const char*> Renderer::GetRequiredExtensions()
{
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
    CheckExtensionsSupport(glfwExtensionCount, glfwExtensions);

    return extensions;
}

void Renderer::CheckExtensionsSupport(uint32_t glfwExtensionCount, const char** glfwExtensions)
{
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
    for (uint32_t i = 0; i < glfwExtensionCount; i++)
    {
        bool isContained = false;
        for (const auto& extension : extensions)
        {
            if (strcmp(glfwExtensions[i], extension.extensionName) == 0)
            {
                isContained = true;
                break;
            }
        }
        if (!isContained)
        {
            Logger::Fatal("glfwExtension is not contained in available extensions");
        }
    }
}

Renderer::Renderer()
{
    std::shared_ptr<RendererSettings> setting = std::make_shared<RendererSettings>();
    settings = DataBinding::Create("Rendering/Settings", setting)->GetData();
    glfwInit();

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = settings->persistence.applicationName.c_str();
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = settings->persistence.applicationName.c_str();
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = vulkanApiVersion;

    auto glfwExtensions = GetRequiredExtensions();

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledLayerCount = 0;
    createInfo.pNext = nullptr;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(glfwExtensions.size());
    createInfo.ppEnabledExtensionNames = glfwExtensions.data();

    if (vkCreateInstance(&createInfo, nullptr, &vkInstance) != VK_SUCCESS)
    {
        Logger::Fatal("failed to create instance!");
    }
    surface = std::make_shared<Surface>(vkInstance);
    device = std::make_shared<Device>(vkInstance, surface);
    CommandResource::CreateGraphicsCommandPool(device);
    commandResource = std::make_shared<CommandResource>(device, Swapchain::MAX_FRAMES_IN_FLIGHT);
    DescriptorResource::CreateDescriptorPool(device);
}

Renderer::~Renderer()
{
    Cleanup();
}

void Renderer::Cleanup()
{
    if (m_cleaned)
    {
        return;
    }
    Shader::Cleanup();
    for (uint32_t i = 0; i < inFlightFences.size(); i++)
    {
        vkDestroyFence(device->vkDevice, inFlightFences[i], nullptr);
    }
    for (uint32_t i = 0; i < renderFinishedSemaphores.size(); i++)
    {
        vkDestroySemaphore(device->vkDevice, renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(device->vkDevice, imageAvailableSemaphores[i], nullptr);
        vkDestroySemaphore(device->vkDevice, gameRenderFinishedSemaphores[i], nullptr);
    }
    if (commandResource != nullptr)
    {
        commandResource->Cleanup();
    }
    for (auto mvp : uniformMvpObjectsNonSolid)
    {
        mvp->Cleanup();
    }
    for (auto mvp : uniformMvpObjects)
    {
        mvp->Cleanup();
    }
    for (auto combinedImage : combinedImages)
    {
        combinedImage->Cleanup();
    }
    for (auto light : uniformLights)
    {
        light->Cleanup();
    }
    if (mainPipeline != nullptr)
    {
        mainPipeline->Cleanup();
    }
    if (nonSolidPipeline != nullptr)
    {
        nonSolidPipeline->Cleanup();
    }
    if (renderPass != nullptr)
    {
        renderPass->Cleanup();
    }
    if (swapchain != nullptr)
    {
        swapchain->Cleanup();
    }
    DescriptorResource::CleanupDescriptorPool(device);
    CommandResource::CleanupGraphicsCommandPool(device);
    device->Cleanup();
    surface->Cleanup();
    vkDestroyInstance(vkInstance, nullptr);
    glfwTerminate();
    m_cleaned = true;
}


void Renderer::Initialize()
{
    swapchain = std::make_shared<Swapchain>(surface, device);
    auto subpass = std::make_shared<Subpass>(device);
    subpass->BeginSubpassAttachments(device->msaaSamples);
    subpass->AddColorAttachment("game", Swapchain::COLOR_FORMAT);
    subpass->AddColorAttachmentResolver("game_resolved");
    subpass->AssignDepthAttachment("game_depth");
    subpass->EndSubpassAttachments();
    renderPass = std::make_shared<RenderPass>(device, subpass);
    swapchain->CreateFramebuffers(renderPass);
    Shader::Initialize();
    auto pbrShader = std::make_shared<Shader>(device);
    std::filesystem::path sourceDir = FileSystemBase::getSourceDir();
    pbrShader->LoadShaderForStage(std::make_shared<File>((sourceDir / "Shaders/PBR.vert").string()), "", VK_SHADER_STAGE_VERTEX_BIT);
    pbrShader->LoadShaderForStage(std::make_shared<File>((sourceDir / "Shaders/PBR.frag").string()), "", VK_SHADER_STAGE_FRAGMENT_BIT);
    mainPipeline = std::make_shared<Pipeline>(device, pbrShader, renderPass, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_POLYGON_MODE_FILL);

    uniformMvpObjects.resize(Swapchain::MAX_FRAMES_IN_FLIGHT);
    combinedImages.resize(Swapchain::MAX_FRAMES_IN_FLIGHT);
    uniformLights.resize(Swapchain::MAX_FRAMES_IN_FLIGHT);
    for (size_t i = 0; i < Swapchain::MAX_FRAMES_IN_FLIGHT; ++i)
    {
        uniformMvpObjects[i] = std::make_shared<UniformBuffer<MVPObject>>(device, mainPipeline->descriptorResource->nameToBinding["MVPObject"],
                                                                          mainPipeline->descriptorResource->vkDescriptorSets[i]);
        combinedImages[i] = std::make_shared<UniformCombinedImage>(device, VK_SAMPLER_ADDRESS_MODE_REPEAT);
        uniformLights[i] = std::make_shared<UniformBuffer<Light>>(device, mainPipeline->descriptorResource->nameToBinding["Light"],
                                                                  mainPipeline->descriptorResource->vkDescriptorSets[i]);
        std::array descriptorWrites{
            uniformMvpObjects[i]->vkWriteDescriptorSet,
            uniformLights[i]->vkWriteDescriptorSet,
        };
        vkUpdateDescriptorSets(device->vkDevice, descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
    }

    auto nonSolidShader = std::make_shared<Shader>(device);
    nonSolidShader->LoadShaderForStage(std::make_shared<File>((sourceDir / "Shaders/FillModeNonSolid.vert").string()), "", VK_SHADER_STAGE_VERTEX_BIT);
    nonSolidShader->LoadShaderForStage(std::make_shared<File>((sourceDir / "Shaders/FillModeNonSolid.frag").string()), "", VK_SHADER_STAGE_FRAGMENT_BIT);
    nonSolidPipeline = std::make_shared<Pipeline>(device, nonSolidShader, renderPass, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_POLYGON_MODE_LINE);
    uniformMvpObjectsNonSolid.resize(Swapchain::MAX_FRAMES_IN_FLIGHT);
    for (size_t i = 0; i < Swapchain::MAX_FRAMES_IN_FLIGHT; ++i)
    {
        uniformMvpObjectsNonSolid[i] = std::make_shared<UniformBuffer<MVPObject>>(device, nonSolidPipeline->descriptorResource->nameToBinding["MVPObject"],
                                                                                  nonSolidPipeline->descriptorResource->vkDescriptorSets[i]);
        std::array descriptorWrites{
            uniformMvpObjectsNonSolid[i]->vkWriteDescriptorSet,
        };
        vkUpdateDescriptorSets(device->vkDevice, descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
    }

    CreateSemaphore(Swapchain::MAX_FRAMES_IN_FLIGHT);
    CreateFence(Swapchain::MAX_FRAMES_IN_FLIGHT);
    mainCamera = std::make_shared<Camera>(DEFAULT_UP, settings->persistence.editorCamera);
}

void Renderer::CreateSemaphore(uint32_t count)
{
    imageAvailableSemaphores.resize(count);
    renderFinishedSemaphores.resize(count);
    gameRenderFinishedSemaphores.resize(count);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    for (size_t i = 0; i < count; i++)
    {
        if (vkCreateSemaphore(device->vkDevice, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(device->vkDevice, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(device->vkDevice, &semaphoreInfo, nullptr, &gameRenderFinishedSemaphores[i]) != VK_SUCCESS
        )
        {
            Logger::Fatal("failed to create synchronization objects for a frame!");
        }
    }
}

void Renderer::CreateFence(uint32_t count)
{
    inFlightFences.resize(count);

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    for (size_t i = 0; i < count; i++)
    {
        if (vkCreateFence(device->vkDevice, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS)
        {
            Logger::Fatal("failed to create synchronization objects for a frame!");
        }
    }
}


void Renderer::BeginDrawFrame()
{
    vkWaitForFences(device->vkDevice, 1, &inFlightFences[frameFlightIndex], VK_TRUE, UINT64_MAX);
    VkResult result = vkAcquireNextImageKHR(device->vkDevice, swapchain->vkSwapchain, UINT64_MAX, imageAvailableSemaphores[frameFlightIndex], VK_NULL_HANDLE, &swapchainImageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        RecreateSwapchain();
        return;
    }
    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        Logger::Fatal("failed to acquire swap chain image!");
    }

    // Only reset the fence if we are submitting work
    VkCommandBuffer currentCommandBuffer = commandResource->vkCommandBuffers[frameFlightIndex];
    vkResetFences(device->vkDevice, 1, &inFlightFences[frameFlightIndex]);
    vkResetCommandBuffer(currentCommandBuffer, 0);
    RecordCommandBuffer(currentCommandBuffer, swapchain->framebuffers[swapchainImageIndex]);


    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[frameFlightIndex]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &currentCommandBuffer;
    VkSemaphore signalSemaphoresGame[] = {gameRenderFinishedSemaphores[frameFlightIndex]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphoresGame;
    if (vkQueueSubmit(device->graphicsQueue, 1, &submitInfo, inFlightFences[frameFlightIndex]) != VK_SUCCESS)
    {
        Logger::Fatal("failed to submit draw command buffer!");
    }
    vkWaitForFences(device->vkDevice, 1, &inFlightFences[frameFlightIndex], VK_TRUE, UINT64_MAX);
    vkResetFences(device->vkDevice, 1, &inFlightFences[frameFlightIndex]);
}

void Renderer::EndDrawFrame()
{
    VkSemaphore presentWaitSemaphores[] = {renderFinishedSemaphores[frameFlightIndex]};
    VkSwapchainKHR swapchains[] = {swapchain->vkSwapchain};
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = presentWaitSemaphores;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapchains;
    presentInfo.pImageIndices = &swapchainImageIndex;
    presentInfo.pResults = nullptr; // Optional
    auto result = vkQueuePresentKHR(device->presentQueue, &presentInfo);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || surface->framebufferResized)
    {
        surface->framebufferResized = false;
        RecreateSwapchain();
    }
    else if (result != VK_SUCCESS)
    {
        Logger::Fatal("failed to present swap chain image!");
    }
    frameFlightIndex = (frameFlightIndex + 1) % Swapchain::MAX_FRAMES_IN_FLIGHT;
}

void Renderer::RecordCommandBuffer(VkCommandBuffer currentCommandBuffer, const std::shared_ptr<Framebuffer>& framebuffer)
{
    auto pipeline = settings->persistence.viewMode == Wireframe ? nonSolidPipeline : mainPipeline;
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0; // Optional
    beginInfo.pInheritanceInfo = nullptr; // Optional

    if (vkBeginCommandBuffer(currentCommandBuffer, &beginInfo) != VK_SUCCESS)
    {
        Logger::Fatal("failed to begin recording command buffer!");
    }

    auto clearColor = settings->persistence.clearColor;
    VkClearColorValue clearColorValue = {{clearColor.r, clearColor.g, clearColor.b, clearColor.a}};
    VkClearDepthStencilValue clearDepthStencilValue = {1.0f, 0};
    renderPass->BeginRenderPass(currentCommandBuffer, framebuffer, swapchain->swapchainVkExtent2D, clearColorValue, clearDepthStencilValue);

    VkViewport viewport;
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(swapchain->swapchainVkExtent2D.width);
    viewport.height = static_cast<float>(swapchain->swapchainVkExtent2D.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(currentCommandBuffer, 0, 1, &viewport);

    VkRect2D scissor;
    scissor.offset = {0, 0};
    scissor.extent = swapchain->swapchainVkExtent2D;
    vkCmdSetScissor(currentCommandBuffer, 0, 1, &scissor);

    // TODO:
    // applicationEditor->grid->uniformBuffer->UpdateMVP(m_currentFrame, editorCamera, glm::mat4(1.0f), projection);
    // applicationEditor->grid->Draw(device, currentCommandBuffer, mainPipeline->descriptorResource, m_currentFrame);

    // 更新 统一 mvp 缓冲区
    auto uniformMvp = settings->persistence.viewMode == Wireframe ? uniformMvpObjectsNonSolid[frameFlightIndex] : uniformMvpObjects[frameFlightIndex];
    MVPObject mvpObject;
    mvpObject.view = mainCamera->GetViewMatrix();
    mvpObject.proj = mainCamera->GetProjectionMatrix();
    mvpObject.proj[1][1] *= -1;

    // 更新 统一 光源 缓冲区
    auto uniformLight = uniformLights[frameFlightIndex];
    Light light;
    light.position = glm::vec3(0.0f, 0.0f, 1.0f);
    light.color = glm::vec3(1.0f);
    uniformLight->AssignData(light);
    vkCmdBindPipeline(currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->vkPipeline);
    if (!pipeline->descriptorResource->vkPushConstantRanges.empty())
    {
        vkCmdPushConstants(currentCommandBuffer, pipeline->vkPipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(glm::vec3), &mainCamera->persistence->position);
        // TODO:支持多个 pushConstants
    }
    vkCmdBindDescriptorSets(currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->vkPipelineLayout, 0, 1,
                            &pipeline->descriptorResource->vkDescriptorSets[frameFlightIndex], 0,
                            nullptr);
    for (auto& renderObject : queuedRenderObjects)
    {
        auto model = renderObject->GetComponent<Model>();
        VkBuffer vertexBuffers[] = {model->vertexBuffer->vkBuffer};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(currentCommandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(currentCommandBuffer, model->indexBuffer->vkBuffer, 0, VK_INDEX_TYPE_UINT32);
        mvpObject.model = renderObject->transform->GetModelMatrix();
        uniformMvp->AssignData(mvpObject);
        vkCmdDrawIndexed(currentCommandBuffer, static_cast<uint32_t>(model->Indices().size()), 1, 0, 0, 0);
    }

    // TODO:recover
    // auto modelMatrix = modelGameObject->transform->GetModelMatrix();
    // applicationEditor->transformGizmo->uniformBuffer->UpdateMVP(m_currentFrame, editorCamera, modelMatrix, projection);
    // applicationEditor->transformGizmo->Draw(editorCamera, currentCommandBuffer, mainPipeline->descriptorResource, m_currentFrame, surface->glfwWindow, projection);
    // Draw(glm::vec3(3, 3, 0), material->irradianceMap->irradianceMap, currentCommandBuffer);

    vkCmdEndRenderPass(currentCommandBuffer);

    if (vkEndCommandBuffer(currentCommandBuffer) != VK_SUCCESS)
    {
        Logger::Fatal("failed to record command buffer!");
    }
}

void Renderer::RecreateSwapchain()
{
    int width = 0, height = 0;
    glfwGetFramebufferSize(surface->glfwWindow, &width, &height);
    while (width == 0 || height == 0)
    {
        glfwGetFramebufferSize(surface->glfwWindow, &width, &height);
        glfwWaitEvents();
    }
    vkDeviceWaitIdle(device->vkDevice);

    swapchain->Cleanup();
    DataBinding::Get<void>("CleanupSwapchain")->Trigger();
    swapchain->CreateSwapchain(surface, device);
    swapchain->CreateFramebuffers(renderPass);
    DataBinding::Get<void>("RecreateSwapchain")->Trigger();
    Logger::Debug("RecreateSwapchain finished");
}
