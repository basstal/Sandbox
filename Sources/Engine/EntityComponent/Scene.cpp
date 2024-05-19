#include "pch.hpp"

#include "Scene.hpp"

#include "Components/Camera.hpp"
#include "Components/Material.hpp"
#include "Components/Mesh.hpp"
#include "Engine/PhysicsSystem.hpp"
#include "Engine/RendererSource/RendererSource.hpp"
#include "Engine/RendererSource/WireframeRendererSource.hpp"
#include "Engine/Skybox.hpp"
#include "Generated/Scene.rfks.h"
#include "VulkanRHI/Renderer.hpp"

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
        auto material = gameObject->GetComponent<Material>();
        if (material == nullptr)
        {
            continue;
        }
        material->Prepare(renderer);
    }
    for (auto& [viewMode, rendererSource] : renderer->rendererSourceMapping)
    {
        rendererSource->RecreateUniformModels(renderer);
    }
    onReconstructMeshes.Trigger();
    auto pbrRendererSource = std::dynamic_pointer_cast<PbrRendererSource>(renderer->rendererSourceMapping[Lit]);
    if (pbrRendererSource != nullptr)
    {
        for (auto& gameObject : rootGameObjects)
        {
            auto material = gameObject->GetComponent<Material>();
            if (material == nullptr)
            {
                continue;
            }
            material->UpdateDescriptorSets(pbrRendererSource);
        }
    }
    auto wireframeRendererSource = std::dynamic_pointer_cast<WireframeRendererSource>(renderer->rendererSourceMapping[Wireframe]);
    if (wireframeRendererSource != nullptr)
    {
        wireframeRendererSource->UpdateDescriptorSets(renderer);
    }
}
// /**
//  * TODO:这里每个材质如果用 gpu instance，需要用数组描述符或动态描述符，
//  * 即使不用 gpu instance，也需要用独立的描述符，除非材质完全共享了资源
//  * @param rendererSource
//  */
// void Sandbox::Scene::UpdateDescriptorSets(const std::shared_ptr<RendererSource>& rendererSource)
// {
//     for (auto& gameObject : rootGameObjects)
//     {
//         auto material = gameObject->GetComponent<Material>();
//         if (material == nullptr)
//         {
//             continue;
//         }
//         material->UpdateDescriptorSets(rendererSource);
//     }
// }

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

void Sandbox::Scene::RemoveGameObject(const std::shared_ptr<Sandbox::GameObject>& gameObject)
{
    // TODO:目前暂时没有 gameObject 父子关系
    rootGameObjects.erase(std::remove(rootGameObjects.begin(), rootGameObjects.end(), gameObject), rootGameObjects.end());
    gameObject->Cleanup();
    onHierarchyChanged.Trigger();
}

std::shared_ptr<Sandbox::Camera> Sandbox::Scene::FindFirstCamera()
{
    for (auto& gameObject : rootGameObjects)
    {
        auto camera = gameObject->GetComponent<Camera>();
        if (camera != nullptr)
        {
            return camera;
        }
        camera = RecursiveFindComponent<Camera>(gameObject);
        if (camera != nullptr)
        {
            return camera;
        }
    }
    return nullptr;
}

std::shared_ptr<Sandbox::GameObject> Sandbox::Scene::CreateGameObject(const std::shared_ptr<GameObject>& copyFrom)
{
    auto gameObject = std::make_shared<GameObject>();
    gameObject->DeserializeFromYaml(copyFrom->SerializeToYaml());
    // gameObject->name                = copyFrom->name;
    // gameObject->transform->position = copyFrom->transform->position;
    // gameObject->transform->rotation = copyFrom->transform->rotation;
    // gameObject->transform->scale    = copyFrom->transform->scale;
    // for (auto& component : copyFrom->GetComponents())
    // {
    //     auto derivedClass = component->GetDerivedClass();
    //     if (derivedClass == nullptr)
    //     {
    //         continue;
    //     }
    //     if (std::string(derivedClass->getName()) == "Transform")
    //     {
    //         continue;  // Transform 类型单独处理
    //     }
    //     auto copiedComponent = derivedClass->makeSharedInstance<IComponent>();
    //     gameObject->AddComponent(copiedComponent);
    //     copiedComponent->DeserializeFromYaml(component->SerializeToYaml());
    // }
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
    currentScene = scene;
    // std::shared_ptr<GameObject> cameraGameObject = std::make_shared<GameObject>();
    // cameraGameObject->name                       = "Camera";
    // auto camera                                  = cameraGameObject->AddComponent<Camera>();
    // scene->rootGameObjects.push_back(cameraGameObject);
    
    onSceneChange.Trigger(scene);
}