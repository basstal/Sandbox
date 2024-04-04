#include "pch.hpp"

#include "Engine.hpp"

#include "Camera.hpp"
#include "Editor/Editor.hpp"
#include "Editor/ImGuiRenderer.hpp"
#include "Editor/ImGuiWindows/Hierarchy.hpp"
#include "Editor/ImGuiWindows/Viewport.hpp"
#include "Editor/TransformGizmo.hpp"
#include "Engine/EntityComponent/Scene.hpp"
#include "EntityComponent/Components/Mesh.hpp"
#include "EntityComponent/Components/Transform.hpp"
#include "EntityComponent/GameObject.hpp"
#include "FileSystem/Directory.hpp"
#include "FileSystem/File.hpp"
#include "Misc/DataBinding.hpp"
#include "Misc/String.hpp"
#include "Model.hpp"
#include "Platform/GlfwCallbackBridge.hpp"
#include "Platform/Window.hpp"
#include "RendererSource/PbrRendererSource.hpp"
#include "RendererSource/UnlitRendererSource.hpp"
#include "RendererSource/WireframeRendererSource.hpp"
#include "Timer.hpp"
#include "VulkanRHI/Common/Debug.hpp"
#include "VulkanRHI/Core/Device.hpp"
#include "VulkanRHI/Renderer.hpp"
#include "VulkanRHI/Rendering/UniformBuffer.hpp"

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

    CreateRenderer();
    CreateEditor();
    DataBinding::Create("Engine/Reload")->Bind([this] { shouldRecreateRenderer = true; });
}


void Sandbox::Engine::CreateEditor()
{
    // 创建和初始化编辑器
    editor = std::make_shared<Editor>();
    timer  = std::make_shared<Timer>();
    timer->SetInterval(60);

    editor->Prepare(renderer, timer, models, window);
}
void Sandbox::Engine::CreateRenderer()
{
    renderer = std::make_shared<Renderer>();
    LOGD("CreateRenderer render ptr {}", PtrToHexString(renderer.get()))
    renderer->Prepare(window);

    auto maxFramesFlight = renderer->maxFramesFlight;
    models.resize(maxFramesFlight);
    for (size_t i = 0; i < maxFramesFlight; ++i)
    {
        models[i] = std::make_shared<Models>();
    }

    auto pbrRendererSource = std::make_shared<PbrRendererSource>();
    pbrRendererSource->Prepare(renderer);
    pbrRendererSource->UpdateModels(renderer, models);
    renderer->rendererSourceMapping[EViewMode::Lit] = pbrRendererSource;

    auto unlitRendererSource = std::make_shared<UnlitRendererSource>();
    unlitRendererSource->Prepare(renderer);
    unlitRendererSource->UpdateModels(renderer, models);
    renderer->rendererSourceMapping[EViewMode::Unlit] = unlitRendererSource;

    auto wireframeRendererSource = std::make_shared<WireframeRendererSource>();
    wireframeRendererSource->Prepare(renderer);
    wireframeRendererSource->UpdateModels(renderer, models);
    renderer->rendererSourceMapping[EViewMode::Wireframe] = wireframeRendererSource;

    renderer->onViewModeChanged.Trigger(Lit);
}

void Sandbox::Engine::MainLoop()
{
    // TODO:临时游戏对象
    std::shared_ptr<Scene>      scene      = std::make_shared<Scene>();
    std::shared_ptr<GameObject> gameObject = std::make_shared<GameObject>();
    gameObject->name                       = "Test";
    Scene::currentScene                    = scene;
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
            std::shared_ptr<RendererSource> rendererSource;
            if (!renderer->TryGetRendererSource(renderer->viewMode, rendererSource))
            {
                LOGF("Renderer source for view mode '{}' not found", VIEW_MODE_NAMES[static_cast<uint32_t>(renderer->viewMode)])
            }
            rendererSource->uboMvp[renderer->frameFlightIndex]->modelsUbo->Update(models[renderer->frameFlightIndex]->model);
            // TODO:临时将 mesh 添加到队列中支持当帧绘制
            renderer->queuedMeshes.push(mesh);
            editor->Draw();
        }
        timer->EndFrame();
        if (shouldRecreateRenderer)
        {
            shouldRecreateRenderer = false;
            editor->Cleanup();
            // TODO:临时放在这里清理
            mesh->Cleanup();
            ValidateVkResult(vkDeviceWaitIdle(renderer->device->vkDevice));
            renderer->Cleanup();
            CreateRenderer();
            CreateEditor();
            // 清理完后重新加载
            mesh->LoadFromModel(renderer->device, renderer->commandBuffers[0], model);
        }
    }
    ValidateVkResult(vkDeviceWaitIdle(renderer->device->vkDevice));
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
    editor->Cleanup();
    renderer->Cleanup();
    glfwTerminate();
}
