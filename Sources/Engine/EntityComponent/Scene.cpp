#include "pch.hpp"

#include "Scene.hpp"

#include "Components/Camera.hpp"
#include "Components/Mesh.hpp"
#include "Engine/RendererSource/RendererSource.hpp"
#include "Generated/Scene.rfks.h"
#include "VulkanRHI/Renderer.hpp"
#include "VulkanRHI/Rendering/UniformBuffer.hpp"

std::shared_ptr<Sandbox::Scene> Sandbox::Scene::currentScene = nullptr;

Sandbox::Event<const std::shared_ptr<Sandbox::Scene>&> Sandbox::Scene::onSceneChange = Sandbox::Event<const std::shared_ptr<Sandbox::Scene>&>();

std::shared_ptr<Sandbox::Scene> Sandbox::Scene::GetCurrentScene() { return currentScene; }

void Sandbox::Scene::Cleanup()
{
    if (m_cleaned)
    {
        return;
    }
    m_cleaned = true;
    for (auto& gameObject : rootGameObjects)
    {
        gameObject->Cleanup();
    }
}
void Sandbox::Scene::Tick(const std::shared_ptr<Renderer>& renderer)
{
    if (models.size() == 0)
    {
        auto maxFramesFlight = renderer->maxFramesFlight;
        models.resize(maxFramesFlight);
        for (size_t i = 0; i < maxFramesFlight; ++i)
        {
            models[i] = std::make_shared<Models>();
        }

        for (auto& [viewMode, rendererSource] : renderer->rendererSourceMapping)
        {
            rendererSource->UpdateModels(renderer, models);
        }
    }
    // TODO:仅处理了第一个 gameobject
    models[renderer->frameFlightIndex]->model[0] = rootGameObjects[0]->transform->GetModelMatrix();

    std::shared_ptr<RendererSource> rendererSource;
    if (!renderer->TryGetRendererSource(renderer->viewMode, rendererSource))
    {
        LOGF_OLD("Renderer source for view mode '{}' not found", VIEW_MODE_NAMES[static_cast<uint32_t>(renderer->viewMode)])
    }
    rendererSource->uboMvp[renderer->frameFlightIndex]->modelsUbo->Update(models[renderer->frameFlightIndex]->model);

    // 将待渲染的 mesh 排队
    auto meshes = std::vector<std::shared_ptr<Mesh>>();
    for (auto& gameObject : rootGameObjects)
    {
        auto mesh = gameObject->GetComponent<Mesh>();
        if (mesh != nullptr)
        {
            meshes.push_back(mesh);
        }
    }
    // TODO:临时将 mesh 添加到队列中支持当帧绘制
    for (auto& mesh : meshes)
    {
        renderer->queuedMeshes.push(mesh);
    }
}

std::shared_ptr<Sandbox::Scene> Sandbox::Scene::LoadScene(std::shared_ptr<File> sceneFile)
{
    std::shared_ptr<Scene> scene = std::make_shared<Scene>();
    scene->LoadFromFile(*sceneFile);
    // std::shared_ptr<GameObject> gameObject       = std::make_shared<GameObject>();
    // gameObject->name                             = "Test";
    // Scene::currentScene                          = scene;
    // scene->rootGameObjects.push_back(gameObject);

    // std::shared_ptr<GameObject> cameraGameObject = std::make_shared<GameObject>();
    // cameraGameObject->name                       = "Camera";
    // auto  camera                                 = cameraGameObject->AddComponent<Camera>();
    // auto& resolution                             = renderer->resolution;
    // auto  aspectRatio                            = static_cast<float>(resolution.width) / static_cast<float>(resolution.height);
    // camera->aspectRatio                          = aspectRatio;
    // static Sandbox::File editorCameraConfigCache = Sandbox::Directory::GetLibraryDirectory().GetFile("EditorCamera.yaml");
    // camera->LoadFromFile(editorCameraConfigCache);
    // camera->UpdateCameraVectors();
    // editor->imGuiRenderer->viewport->mainCamera = camera;
    // scene->rootGameObjects.push_back(cameraGameObject);
    //
    // // TODO: 测试保存场景
    // auto sceneFile = Directory::GetAssetsDirectory().GetFile("Test.scene");
    // LOGD("Test", "begin serialize scene")
    // scene->SaveToFile(sceneFile);
    // // LOGD("end serialize scene")
    // // scene->LoadFromFile(sceneFile);
    // editor->imGuiRenderer->hierarchy->SetScene(scene);
    // auto mesh = gameObject->AddComponent<Mesh>();
    // // gameObjects.push_back(gameObject);
    // // TODO:临时加载模型
    // Model model(Directory::GetAssetsDirectory().GetFile("Models/viking_room.obj").path.string());
    // mesh->LoadFromModel(renderer->device, renderer->commandBuffers[0], model);
    return scene;
}


void Sandbox::Scene::NewScene()
{
    // TODO:临时游戏对象
    std::shared_ptr<Scene> scene = std::make_shared<Scene>();
    // std::shared_ptr<GameObject> gameObject = std::make_shared<GameObject>();
    // gameObject->name                       = "Test";
    // Scene::currentScene                    = scene;
    // scene->rootGameObjects.push_back(gameObject);

    std::shared_ptr<GameObject> cameraGameObject = std::make_shared<GameObject>();
    cameraGameObject->name                       = "Camera";
    auto camera                                  = cameraGameObject->AddComponent<Camera>();
    // auto& resolution                             = renderer->resolution;
    // auto  aspectRatio                            = static_cast<float>(resolution.width) / static_cast<float>(resolution.height);
    // camera->aspectRatio                          = aspectRatio;
    // static Sandbox::File editorCameraConfigCache = Sandbox::Directory::GetLibraryDirectory().GetFile("EditorCamera.yaml");
    // camera->LoadFromFile(editorCameraConfigCache);
    // camera->UpdateCameraVectors();
    // editor->imGuiRenderer->viewport->mainCamera = camera;
    // scene->rootGameObjects.push_back(cameraGameObject);
    //
    // // TODO: 测试保存场景
    // // auto sceneFile = Directory::GetAssetsDirectory().GetFile("Test.scene");
    // // LOGD("Test", "begin serialize scene")
    // // scene->SaveToFile(sceneFile);
    // // LOGD("end serialize scene")
    // // scene->LoadFromFile(sceneFile);
    // editor->imGuiRenderer->hierarchy->SetScene(scene);
    // auto mesh = gameObject->AddComponent<Mesh>();
    // gameObjects.push_back(gameObject);
    // TODO:临时加载模型
    // Model model(Directory::GetAssetsDirectory().GetFile("Models/viking_room.obj").path.string());
    // mesh->LoadFromModel(renderer->device, renderer->commandBuffers[0], model);
}