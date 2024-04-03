#include "pch.hpp"

#include "Engine.hpp"

#include "Camera.hpp"
#include "Image.hpp"
#include "Light.hpp"
#include "Model.hpp"
#include "Timer.hpp"
#include "Editor/Editor.hpp"
#include "Editor/ImGuiRenderer.hpp"
#include "Editor/TransformGizmo.hpp"
#include "Editor/ImGuiWindows/Viewport.hpp"
#include "EntityComponent/GameObject.hpp"
#include "EntityComponent/Components/Mesh.hpp"
#include "EntityComponent/Components/Transform.hpp"
#include "FileSystem/Directory.hpp"
#include "FileSystem/File.hpp"
#include "Platform/GlfwCallbackBridge.hpp"
#include "Misc/DataBinding.hpp"
#include "Misc/Memory.hpp"
#include "Platform/Window.hpp"
#include "VulkanRHI/Renderer.hpp"
#include "VulkanRHI/Common/Debug.hpp"
#include "VulkanRHI/Core/Buffer.hpp"
#include "VulkanRHI/Core/CommandBuffer.hpp"
#include "VulkanRHI/Core/DescriptorSet.hpp"
#include "VulkanRHI/Core/Device.hpp"
#include "VulkanRHI/Core/Image.hpp"
#include "VulkanRHI/Core/ImageView.hpp"
#include "VulkanRHI/Core/Instance.hpp"
#include "VulkanRHI/Core/Pipeline.hpp"
#include "VulkanRHI/Core/PipelineLayout.hpp"
#include "VulkanRHI/Core/Surface.hpp"
#include "VulkanRHI/Core/Swapchain.hpp"
#include "VulkanRHI/Rendering/Texture.hpp"
#include "VulkanRHI/Rendering/UniformBuffer.hpp"
#include "Engine/EntityComponent/Scene.hpp"
#include "Editor/ImGuiWindows/Hierarchy.hpp"
#include "Misc/String.hpp"

int main()
{
    try
    {
        Sandbox::Engine engine;
        engine.Prepare();
        engine.MainLoop();
        engine.Cleanup();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return -1;
    }
    return 0;
}


void Sandbox::Engine::Prepare()
{
    // 初始化 glfw
    if (!glfwInit())
    {
        LOGF("glfw init failed!")
    }
    // 创建和初始化渲染器
    window = std::make_shared<Window>();
    window->Prepare();
    renderer = std::make_shared<Renderer>();
    renderer->Prepare(window);
    // 创建和初始化编辑器
    editor = std::make_shared<Editor>();
    timer = std::make_shared<Timer>();

    std::vector<std::array<std::shared_ptr<Texture>, 4>> textures(renderer->maxFramesFlight);
    std::shared_ptr<Sandbox::Resource::Image> albedo = std::make_shared<Resource::Image>(Directory::GetAssetsDirectory().GetFile("Textures/pbr/rusted_iron/albedo.png"));
    std::shared_ptr<Sandbox::Resource::Image> metallic = std::make_shared<Resource::Image>(Directory::GetAssetsDirectory().GetFile("Textures/pbr/rusted_iron/metallic.png"));
    std::shared_ptr<Sandbox::Resource::Image> roughness = std::make_shared<Resource::Image>(Directory::GetAssetsDirectory().GetFile("Textures/pbr/rusted_iron/roughness.png"));
    std::shared_ptr<Sandbox::Resource::Image> ao = std::make_shared<Resource::Image>(Directory::GetAssetsDirectory().GetFile("Textures/pbr/rusted_iron/ao.png"));

    auto albedoImage = std::make_shared<Image>(renderer->device, VkExtent3D(albedo->width, albedo->height, 1), VK_FORMAT_R8G8B8A8_UNORM,
                                               VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, VK_MEMORY_HEAP_DEVICE_LOCAL_BIT,
                                               VK_SAMPLE_COUNT_1_BIT,
                                               VK_IMAGE_TILING_OPTIMAL, albedo->mipLevels);
    auto metallicImage = std::make_shared<Image>(renderer->device, VkExtent3D(metallic->width, metallic->height, 1), VK_FORMAT_R8G8B8A8_UNORM,
                                                 VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, VK_MEMORY_HEAP_DEVICE_LOCAL_BIT,
                                                 VK_SAMPLE_COUNT_1_BIT,
                                                 VK_IMAGE_TILING_OPTIMAL, metallic->mipLevels);
    auto roughnessImage = std::make_shared<Image>(renderer->device, VkExtent3D(roughness->width, roughness->height, 1), VK_FORMAT_R8G8B8A8_UNORM,
                                                  VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, VK_MEMORY_HEAP_DEVICE_LOCAL_BIT,
                                                  VK_SAMPLE_COUNT_1_BIT,
                                                  VK_IMAGE_TILING_OPTIMAL, roughness->mipLevels);
    auto aoImage = std::make_shared<Image>(renderer->device, VkExtent3D(ao->width, ao->height, 1), VK_FORMAT_R8G8B8A8_UNORM,
                                           VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
                                           VK_MEMORY_HEAP_DEVICE_LOCAL_BIT, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL, ao->mipLevels);

    renderer->commandBuffers[0]->CopyDataToImage(albedo, albedoImage, VK_FORMAT_R8G8B8A8_UNORM);
    renderer->commandBuffers[0]->CopyDataToImage(metallic, metallicImage, VK_FORMAT_R8G8B8A8_UNORM);
    renderer->commandBuffers[0]->CopyDataToImage(roughness, roughnessImage, VK_FORMAT_R8G8B8A8_UNORM);
    renderer->commandBuffers[0]->CopyDataToImage(ao, aoImage, VK_FORMAT_R8G8B8A8_UNORM);

    for (size_t i = 0; i < textures.size(); ++i)
    {
        textures[i][0] = std::make_shared<Texture>(renderer->device, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
        textures[i][0]->image = albedoImage;
        textures[i][0]->imageView = std::make_shared<ImageView>(albedoImage, VK_IMAGE_VIEW_TYPE_2D);
        textures[i][1] = std::make_shared<Texture>(renderer->device, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
        textures[i][1]->image = metallicImage;
        textures[i][1]->imageView = std::make_shared<ImageView>(metallicImage, VK_IMAGE_VIEW_TYPE_2D);
        textures[i][2] = std::make_shared<Texture>(renderer->device, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
        textures[i][2]->image = roughnessImage;
        textures[i][2]->imageView = std::make_shared<ImageView>(roughnessImage, VK_IMAGE_VIEW_TYPE_2D);
        textures[i][3] = std::make_shared<Texture>(renderer->device, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
        textures[i][3]->image = aoImage;
        textures[i][3]->imageView = std::make_shared<ImageView>(aoImage, VK_IMAGE_VIEW_TYPE_2D);
    }
    renderer->textures = textures;
    auto maxFramesFlight = renderer->maxFramesFlight;
    models.resize(maxFramesFlight);
    for (size_t i = 0; i < maxFramesFlight; ++i)
    {
        models[i] = std::make_shared<Models>();
        renderer->uboMvp[i] = PrepareUniformBuffers(models[i]);
    }
    editor->Prepare(renderer, timer, models, window);

    uint32_t dynamicAlignment = editor->GetUniformDynamicAlignment(sizeof(glm::mat4));
    for (size_t i = 0; i < maxFramesFlight; ++i)
    {
        renderer->uboLights[i] = std::make_shared<UniformBuffer>(renderer->device, sizeof(Light));
        BindingMap<VkDescriptorBufferInfo> bufferInfoMapping = {
            {0, {renderer->uboMvp[i]->vpUbo->GetDescriptorBufferInfo()}},
            {1, {renderer->uboMvp[i]->modelsUbo->GetDescriptorBufferInfo(dynamicAlignment)}},
            {3, {renderer->uboLights[i]->GetDescriptorBufferInfo()}},
        };

        BindingMap<VkDescriptorImageInfo> imageInfoMapping = {
            {2, {textures[i][0]->GetDescriptorImageInfo(), textures[i][1]->GetDescriptorImageInfo(), textures[i][2]->GetDescriptorImageInfo(), textures[i][3]->GetDescriptorImageInfo()}},
        };
        renderer->descriptorSets[i] = std::make_shared<DescriptorSet>(renderer->device, renderer->descriptorPool, renderer->pipelineLayout->descriptorSetLayout, bufferInfoMapping,
                                                                      imageInfoMapping);
    }
    editor->PrepareOnGui();
}

std::shared_ptr<Sandbox::MVPUboObjects> Sandbox::Engine::PrepareUniformBuffers(const std::shared_ptr<Models>& inModels)
{
    auto mvpUboObjects = std::make_shared<MVPUboObjects>();
    mvpUboObjects->vpUbo = std::make_shared<UniformBuffer>(renderer->device, sizeof(ViewAndProjection));
    renderer->viewAndProjection = std::make_shared<ViewAndProjection>();
    mvpUboObjects->vpUbo->Update(&*renderer->viewAndProjection);

    // Calculate required alignment based on minimum device offset alignment
    VkDeviceSize dynamicAlignment = sizeof(glm::mat4);
    renderer->device->GetMinUniformBufferOffsetAlignment(dynamicAlignment);
    constexpr uint32_t objectCount = 1 + 2;
    VkDeviceSize modelsBufferSize = objectCount * dynamicAlignment;
    inModels->model = static_cast<glm::mat4*>(AlignedAlloc(modelsBufferSize, dynamicAlignment));
    inModels->model[0] = glm::translate(glm::mat4(1.0f), glm::vec3(1.5f, 0.0f, 0.0f));
    inModels->model[1] = glm::mat4(1.0f); // NOTE:rid 一直处在场景中央
    mvpUboObjects->modelsUbo = std::make_shared<UniformBuffer>(renderer->device, modelsBufferSize);
    mvpUboObjects->modelsUbo->Update(inModels->model);
    return mvpUboObjects;
}

void Sandbox::Engine::MainLoop()
{
    // TODO:临时游戏对象
    std::shared_ptr<Scene> scene = std::make_shared<Scene>();
    std::shared_ptr<GameObject> gameObject = std::make_shared<GameObject>();
    gameObject->name = "Test";
    Scene::currentScene = scene;
    scene->rootGameObjects.push_back(gameObject);
    // TODO: 测试保存场景
    auto sceneFile = Directory::GetAssetsDirectory().GetFile("Test.scene");
    // LOGD("begin serialize scene")
    // scene->SaveToFile(sceneFile);
    // LOGD("end serialize scene")
    // scene->LoadFromFile(sceneFile);
    editor->imGuiRenderer->hierarchy->SetScene(scene);
    auto mesh = gameObject->AddComponent<Mesh>();
    // gameObjects.push_back(gameObject);
    // TODO:临时加载模型
    Model model(Directory::GetAssetsDirectory().GetFile("Models/viking_room.obj").path.string());
    mesh->LoadFromModel(renderer->device, renderer->commandBuffers[0], model);
    // editor->transformGizmo->SetTarget(gameObject);
    editor->imGuiRenderer->viewport->SetTarget(gameObject);
    timer->SetInterval(60);
    window->callbackBridge->onKey.BindMember<Engine, &Engine::Pause>(this);
    while (!glfwWindowShouldClose(window->glfwWindow))
    {
        glfwPollEvents();
        editor->Update();
        if (pause)
        {
            continue;
        }
        if (timer->ShouldTickFrame())
        {
            models[renderer->frameFlightIndex]->model[0] = gameObject->transform->GetModelMatrix();
            renderer->uboMvp[renderer->frameFlightIndex]->modelsUbo->Update(models[renderer->frameFlightIndex]->model);
            // TODO:临时将 mesh 添加到队列中支持当帧绘制
            renderer->queuedMeshes.push(mesh);
            editor->Draw();
        }
        timer->EndFrame();
    }
    ValidateVkResult(vkDeviceWaitIdle(renderer->device->vkDevice));
    // for (auto& gameObjectToClean : gameObjects)
    // {
    //     gameObjectToClean->Cleanup();
    // }
    scene->Cleanup();
}

void Sandbox::Engine::Pause(GLFWwindow* inWindow, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_P && action == GLFW_PRESS)
    {
        pause = !pause;
    }
}

void Sandbox::Engine::Cleanup()
{
    window->Cleanup();
    auto maxFramesFlight = renderer->maxFramesFlight;
    editor->Cleanup();
    for (size_t i = 0; i < maxFramesFlight; ++i)
    {
        renderer->descriptorSets[i]->Cleanup();
        for (size_t t = 0; t < 4; ++t)
        {
            renderer->textures[i][t]->Cleanup();
        }
        renderer->uboLights[i]->Cleanup();
        renderer->uboMvp[i]->modelsUbo->Cleanup();
        renderer->uboMvp[i]->vpUbo->Cleanup();
    }
    renderer->Cleanup();
    glfwTerminate();
}
