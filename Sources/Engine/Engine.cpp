#include "pch.hpp"

#include "Engine.hpp"

#include "Editor/Editor.hpp"
#include "Editor/ImGuiRenderer.hpp"
#include "Editor/ImGuiWindows/Hierarchy.hpp"
#include "Editor/ImGuiWindows/Viewport.hpp"
#include "Editor/TransformGizmo.hpp"
#include "Engine/EntityComponent/Components/Mesh.hpp"
#include "Engine/EntityComponent/Scene.hpp"
#include "EntityComponent/Components/Camera.hpp"
#include "FileSystem/Directory.hpp"
#include "Misc/DataBinding.hpp"
#include "Misc/TypeCasting.hpp"
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
        LOGF_OLD("glfw init failed!")
    }
    // 创建和初始化渲染器
    window = std::make_shared<Window>();
    window->Prepare();

    CreateRenderer();
    CreateEditor();
    DataBinding::Create("Engine/Reload")->Bind([this] { shouldRecreateRenderer = true; });
    // DataBinding::Create("Engine/NewScene")->Bind([this] { this->NewScene(); });
}

// void Sandbox::Engine::SetScene(const std::shared_ptr<Scene>& inScene) { scene = inScene; }


void Sandbox::Engine::CreateEditor()
{
    // 创建和初始化编辑器
    editor        = std::make_shared<Editor>();
    rendererTimer = std::make_shared<Timer>();
    logicTimer    = std::make_shared<Timer>();
    editor->Prepare(renderer, rendererTimer, models, window);
}
void Sandbox::Engine::CreateRenderer()
{
    renderer = std::make_shared<Renderer>();
    LOGD_OLD("CreateRenderer render ptr {}", PtrToHexString(renderer.get()))
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
    // auto scene = std::make_shared<Scene>();
    // // TODO:临时游戏对象
    // std::shared_ptr<GameObject> gameObject = std::make_shared<GameObject>();
    // gameObject->name                       = "Test";
    // Scene::currentScene                    = scene;
    // scene->rootGameObjects.push_back(gameObject);
    //
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
    window->callbackBridge->onKey.BindMember<Engine, &Engine::Pause>(this);
    auto                      fpsLimit = 60;
    std::chrono::microseconds logicUpdateDeltaTime(ToUInt32(std::floor(1000000 / fpsLimit)));
    std::chrono::microseconds rendererUpdateInterval(ToUInt32(std::floor(1000000 / fpsLimit)));
    while (!glfwWindowShouldClose(window->glfwWindow))
    {
        glfwPollEvents();
        editor->Update();

        if (pause)
        {
            continue;
        }

        // 逻辑以固定步长（时长）更新
        logicTimer->UpdateInFixed(logicUpdateDeltaTime);
        while (logicTimer->BeginFixed())
        {
            if (Scene::currentScene != nullptr)
            {
                Scene::currentScene->Tick(renderer);
            }
            logicTimer->EndFixed();
        }

        // 渲染以固定帧率更新
        if (rendererTimer->UpdateInInterval(rendererUpdateInterval))
        {
            editor->Draw();
        }
        rendererTimer->EndFrame();
        // TODO:重构这个功能
        if (shouldRecreateRenderer)
        {
            shouldRecreateRenderer = false;
            Logger::onLogMessage.Cleanup();
            editor->Cleanup();
            // // TODO:临时放在这里清理
            // mesh->Cleanup();
            ValidateVkResult(vkDeviceWaitIdle(renderer->device->vkDevice));
            renderer->Cleanup();
            CreateRenderer();
            CreateEditor();
            // // 清理完后重新加载
            // mesh->LoadFromModel(renderer->device, renderer->commandBuffers[0], model);
            // editor->imGuiRenderer->viewport->mainCamera = camera;
        }
    }
    ValidateVkResult(vkDeviceWaitIdle(renderer->device->vkDevice));
    if (Scene::currentScene != nullptr)
    {
        Scene::currentScene->Cleanup();
    }
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
