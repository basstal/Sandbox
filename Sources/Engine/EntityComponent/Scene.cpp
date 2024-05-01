#include "pch.hpp"

#include "Scene.hpp"

#include "Components/Camera.hpp"
#include "Components/Material.hpp"
#include "Components/Mesh.hpp"
#include "Engine/PhysicsSystem.hpp"
#include "Engine/RendererSource/RendererSource.hpp"
#include "Engine/Skybox.hpp"
#include "Generated/Scene.rfks.h"
#include "VulkanRHI/Renderer.hpp"
#include "VulkanRHI/Rendering/UniformBuffer.hpp"

std::shared_ptr<Sandbox::Scene> Sandbox::Scene::currentScene = nullptr;

Sandbox::Event<const std::shared_ptr<Sandbox::Scene>&> Sandbox::Scene::onSceneChange = Sandbox::Event<const std::shared_ptr<Sandbox::Scene>&>();

Sandbox::Event<void> Sandbox::Scene::onReconstructMeshes = Sandbox::Event<void>();

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
    if (currentScene.get() == this)  // 防止 currentScene 引用已清理对象
    {
        currentScene = nullptr;
    }
}


void Sandbox::Scene::Tick(const std::shared_ptr<Renderer>& renderer) {}

void Sandbox::Scene::ReconstructMeshes(const std::shared_ptr<Renderer>& renderer)
{
    if (!isRenderMeshesDirty)
    {
        return;
    }
    isRenderMeshesDirty = false;
    // 将待渲染的 mesh 排队
    renderMeshes.clear();
    if (models.empty())
    {
        auto maxFramesFlight = renderer->maxFramesFlight;
        models.resize(maxFramesFlight);
        for (size_t i = 0; i < maxFramesFlight; ++i)
        {
            models[i] = std::make_shared<Models>();
        }
    }

    for (auto& gameObject : rootGameObjects)
    {
        auto mesh = gameObject->GetComponent<Mesh>();
        if (mesh == nullptr)
        {
            continue;
        }
        if (mesh->SubmitModelToDevice(renderer->device, renderer->commandBuffers[0]))
        {
            mesh->RegisterModelToPhysicsWorld();
            renderMeshes.push_back(mesh);
        }
    }
    for (auto& [viewMode, rendererSource] : renderer->rendererSourceMapping)
    {
        rendererSource->RecreateUniformModels(renderer);
    }
    onReconstructMeshes.Trigger();
}

void Sandbox::Scene::TranslateRenderData(const std::shared_ptr<Renderer>& renderer)
{
    ReconstructMeshes(renderer);

    // 复制模型世界坐标数据到 UBO
    std::shared_ptr<RendererSource> rendererSource;
    if (!renderer->TryGetRendererSource(renderer->viewMode, rendererSource))
    {
        LOGF_OLD("Renderer source for view mode '{}' not found", VIEW_MODE_NAMES[static_cast<uint32_t>(renderer->viewMode)])
    }
    rendererSource->Tick(renderer);
    onOtherRendererSourceTick.Trigger(renderer);

    // TODO:临时将 mesh 添加到队列中支持当帧绘制
    // TODO:天空盒等其他需要在场景中绘制的对象也可以添加为 Mesh，就是需要改几何数据集成的地方
    for (auto& mesh : renderMeshes)
    {
        if (!mesh->IsValid())
        {
            LOGF("Engine", "Mesh {} is not valid!!", mesh->gameObject.lock()->name.ToStdString())
        }
        renderer->queuedMaterials.push_back(mesh->material);
    }
}


std::shared_ptr<Sandbox::GameObject> Sandbox::Scene::AddEmptyGameObject(const std::string& name, const Vector3& position)
{
    auto gameObject                 = std::make_shared<GameObject>();
    gameObject->name                = name;
    gameObject->transform->position = position;
    rootGameObjects.push_back(gameObject);
    onHierarchyChanged.Trigger();
    return gameObject;
}

std::shared_ptr<Sandbox::GameObject> Sandbox::Scene::AddEmptyGameObject()
{
    auto gameObject = std::make_shared<GameObject>();
    rootGameObjects.push_back(gameObject);
    onHierarchyChanged.Trigger();
    return gameObject;
}

std::shared_ptr<Sandbox::Scene> Sandbox::Scene::LoadScene(std::shared_ptr<File> sceneFile)
{
    std::shared_ptr<Scene> scene = std::make_shared<Scene>();
    if (Scene::currentScene != nullptr)
    {
        Scene::currentScene->Cleanup();
    }
    Scene::currentScene = scene;
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

    Scene::onSceneChange.Trigger(scene);
    return scene;
}


void Sandbox::Scene::NewScene()
{
    // TODO:临时游戏对象
    std::shared_ptr<Scene> scene = std::make_shared<Scene>();
    if (Scene::currentScene != nullptr)
    {
        Scene::currentScene->Cleanup();
    }
    currentScene                                 = scene;
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
    
    onSceneChange.Trigger(scene);
}