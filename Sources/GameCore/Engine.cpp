#include "Engine.hpp"

#include "Model.hpp"
#include "Scene.hpp"
#include "Editor/ApplicationEditor.hpp"
#include "Editor/OverlayEditor.hpp"
#include "Editor/SettingsEditor.hpp"
#include "Editor/ValueEditor.hpp"
#include "Infrastructures/DataBinding.hpp"
#include "Infrastructures/SingletonOrganizer.hpp"
#include "Rendering/Application.hpp"

void Engine::Initialize()
{
	SingletonOrganizer::Register<Timer>();
	SingletonOrganizer::Register<Application>();
	auto application = SingletonOrganizer::Get<Application>();
	application->Initialize();
	std::shared_ptr<ApplicationEditor> applicationEditor = std::make_shared<ApplicationEditor>();
	DataBinding::Create("ApplicationEditor", applicationEditor);
	SettingsEditor::Create(applicationEditor);
	ValueEditor::Create(applicationEditor);
	OverlayEditor::Create(applicationEditor);
}
void Engine::MainLoop()
{
	auto application = SingletonOrganizer::Get<Application>();
	auto applicationEditor = DataBinding::Get<std::shared_ptr<ApplicationEditor>>("ApplicationEditor")->GetData();
	auto timer = SingletonOrganizer::Get<Timer>();
	auto scene = std::make_shared<Scene>();
	// TODO:remove test code
	auto gameobject = std::make_shared<GameObject>();
	auto model = std::make_shared<Model>();
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
	auto application = SingletonOrganizer::Get<Application>();
	auto applicationEditor = DataBinding::Get<std::shared_ptr<ApplicationEditor>>("ApplicationEditor")->GetData();
	vkDeviceWaitIdle(application->device->vkDevice);
	applicationEditor->Cleanup();
	application->Cleanup();
}
