#include "Engine.hpp"

#include <filesystem>

#include "GameObject.hpp"
#include "Material.hpp"
#include "Model.hpp"
#include "Scene.hpp"
#include "Editor/ApplicationEditor.hpp"
#include "Editor/OverlayEditor.hpp"
#include "Editor/SettingsEditor.hpp"
#include "Editor/ValueEditor.hpp"
#include "Infrastructures/DataBinding.hpp"
#include "Infrastructures/SingletonOrganizer.hpp"
#include "Infrastructures/Timer.hpp"
#include "Infrastructures/FileSystem/FileSystemBase.hpp"
#include "Rendering/Renderer.hpp"
#include "Rendering/Buffers/Image.hpp"
#include "Rendering/Buffers/UniformCombinedImage.hpp"
#include "Rendering/Components/DescriptorResource.hpp"
#include "Rendering/Components/Pipeline.hpp"
#include "Resources/Image.hpp"
#include "Rendering/Components/Swapchain.hpp"
#include "Rendering/Base/Surface.hpp"

void Engine::Initialize()
{
    SingletonOrganizer::Register<Timer>();
    SingletonOrganizer::Register<Renderer>();
    auto application = SingletonOrganizer::Get<Renderer>();
    application->Initialize();
    std::shared_ptr<ApplicationEditor> applicationEditor = std::make_shared<ApplicationEditor>();
    DataBinding::Create("ApplicationEditor", applicationEditor);
    SettingsEditor::Create(applicationEditor);
    ValueEditor::Create(applicationEditor);
    OverlayEditor::Create(applicationEditor);
}

void Engine::MainLoop()
{
    auto renderer = SingletonOrganizer::Get<Renderer>();
    auto applicationEditor = DataBinding::Get<std::shared_ptr<ApplicationEditor>>("ApplicationEditor")->GetData();
    auto timer = SingletonOrganizer::Get<Timer>();
    scene = std::make_shared<Scene>();
    // TODO:remove test code
    auto gameobject = std::make_shared<GameObject>();
    auto assetPath = FileSystemBase::getAssetsDir();
    auto model = Model::LoadModel((assetPath + "/Models/viking_room.obj").c_str());
    gameobject->AddComponent(model);
    auto usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    std::filesystem::path assetsDir = FileSystemBase::getAssetsDir();
    auto albedo = GameCore::Image::LoadImage((assetsDir / "Textures/pbr/rusted_iron/albedo.png").string().c_str());
    auto albedoImage = std::make_shared<Image>(renderer->device, albedo->Width(), albedo->Height(), albedo->MipLevels(), VK_SAMPLE_COUNT_1_BIT, Swapchain::COLOR_FORMAT,
                                               VK_IMAGE_TILING_OPTIMAL, usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, false, VK_IMAGE_ASPECT_COLOR_BIT);
    albedoImage->AssignData(albedo, Swapchain::COLOR_FORMAT, false);

    auto metallic = GameCore::Image::LoadImage((assetsDir / "Textures/pbr/rusted_iron/metallic.png").string().c_str());
    auto metallicImage = std::make_shared<Image>(renderer->device, metallic->Width(), metallic->Height(), metallic->MipLevels(), VK_SAMPLE_COUNT_1_BIT, Swapchain::COLOR_FORMAT,
                                                 VK_IMAGE_TILING_OPTIMAL, usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, false, VK_IMAGE_ASPECT_COLOR_BIT);
    metallicImage->AssignData(metallic, Swapchain::COLOR_FORMAT, false);

    auto roughness = GameCore::Image::LoadImage((assetsDir / "Textures/pbr/rusted_iron/roughness.png").string().c_str());
    auto roughnessImage = std::make_shared<Image>(renderer->device, roughness->Width(), roughness->Height(), roughness->MipLevels(), VK_SAMPLE_COUNT_1_BIT, Swapchain::COLOR_FORMAT,
                                                  VK_IMAGE_TILING_OPTIMAL, usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, false, VK_IMAGE_ASPECT_COLOR_BIT);
    roughnessImage->AssignData(roughness, Swapchain::COLOR_FORMAT, false);

    auto ao = GameCore::Image::LoadImage((assetsDir / "Textures/pbr/rusted_iron/ao.png").string().c_str());
    auto aoImage = std::make_shared<Image>(renderer->device, ao->Width(), ao->Height(), ao->MipLevels(), VK_SAMPLE_COUNT_1_BIT, Swapchain::COLOR_FORMAT,
                                           VK_IMAGE_TILING_OPTIMAL, usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, false, VK_IMAGE_ASPECT_COLOR_BIT);
    aoImage->AssignData(ao, Swapchain::COLOR_FORMAT, false);

    auto irradiance = GameCore::Image::LoadHdrImage((assetsDir / "Textures/hdr/newport_loft.hdr").string().c_str());
    auto material = std::make_shared<Material>(renderer->device, albedo, metallic, roughness, ao, irradiance, renderer->commandResource, renderer->mainPipeline, renderer->renderPass,
                                               renderer->mainPipeline->descriptorResource);
    gameobject->AddComponent(material);
    scene->AddGameObject(gameobject);
    for (uint32_t i = 0; i < renderer->combinedImages.size(); ++i)
    {
        auto& combinedImage = renderer->combinedImages[i];
        combinedImage->AddImage(albedoImage);
        combinedImage->AddImage(metallicImage);
        combinedImage->AddImage(roughnessImage);
        combinedImage->AddImage(aoImage);
        std::vector<VkDescriptorImageInfo> descriptorImageInfos = combinedImage->CreateDescriptorImageInfos();
        auto writeDescriptorSet = combinedImage->CreateWriteDescriptorSet(renderer->mainPipeline->descriptorResource->nameToBinding["textures"],
                                                                          renderer->mainPipeline->descriptorResource->vkDescriptorSets[i], descriptorImageInfos);
        vkUpdateDescriptorSets(renderer->device->vkDevice, 1, &writeDescriptorSet, 0, nullptr);
    }
    while (!glfwWindowShouldClose(renderer->surface->glfwWindow))
    {
        glfwPollEvents();
        timer->SetInterval(60);
        if (timer->ShouldTickFrame())
        {
            scene->Update();
            renderer->BeginDrawFrame();
            applicationEditor->DrawFrame();
            renderer->EndDrawFrame();
            timer->EndFrame();
        }
    }
}

void Engine::Cleanup()
{
    auto renderer = SingletonOrganizer::Get<Renderer>();
    auto applicationEditor = DataBinding::Get<std::shared_ptr<ApplicationEditor>>("ApplicationEditor")->GetData();
    vkDeviceWaitIdle(renderer->device->vkDevice);
    scene->Cleanup();
    applicationEditor->Cleanup();
    renderer->Cleanup();
}
