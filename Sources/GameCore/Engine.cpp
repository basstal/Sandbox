#include "Engine.hpp"

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
    auto application = SingletonOrganizer::Get<Renderer>();
    auto applicationEditor = DataBinding::Get<std::shared_ptr<ApplicationEditor>>("ApplicationEditor")->GetData();
    auto timer = SingletonOrganizer::Get<Timer>();
    auto scene = std::make_shared<Scene>();
    // TODO:remove test code
    auto gameobject = std::make_shared<GameObject>();
    auto assetPath = FileSystemBase::getAssetsDir();
    auto model = Model::LoadModel((assetPath + "/Models/viking_room.obj").c_str());
    gameobject->AddComponent(model);
    scene->AddGameObject(gameobject);
    while (!glfwWindowShouldClose(application->surface->glfwWindow))
    {
        glfwPollEvents();
        timer->SetInterval(60);
        if (timer->ShouldTickFrame())
        {
            scene->Update();
            application->DrawFrame(applicationEditor);
            timer->EndFrame();
        }
    }
}

void Engine::Cleanup()
{
    auto application = SingletonOrganizer::Get<Renderer>();
    auto applicationEditor = DataBinding::Get<std::shared_ptr<ApplicationEditor>>("ApplicationEditor")->GetData();
    vkDeviceWaitIdle(application->device->vkDevice);
    applicationEditor->Cleanup();
    application->Cleanup();
}
