#pragma once
#include <memory>

#include "Engine/Concepts.hpp"
#include "GameObject.hpp"
#include "Generated/Scene.rfkh.h"
#include "Serialization/ISerializable.hpp"
#include "Serialization/List.hpp"

namespace Sandbox NAMESPACE()
{
    struct Models;
    class Renderer;
    class Skybox;
    class Camera;

    class Mesh;

    class CLASS() Scene : public ISerializable  //<Scene>
    {
    public:
        static std::shared_ptr<Scene>               currentScene;
        static Event<const std::shared_ptr<Scene>&> onSceneChange;
        static Event<void>                          onReconstructMeshes;

        static std::shared_ptr<Scene> GetCurrentScene();
        static std::shared_ptr<Scene> LoadScene(std::shared_ptr<File> sceneFile);

        static void NewScene();


        FIELD()
        List<SharedPtr<GameObject>> rootGameObjects;

        std::vector<std::shared_ptr<Mesh>> renderMeshes;

        bool isRenderMeshesDirty = true;

        std::vector<std::shared_ptr<Models>> models;

        Event<const std::shared_ptr<Renderer>&> onOtherRendererSourceTick;
        Event<void>                             onHierarchyChanged;


        void Cleanup();

        void Tick(const std::shared_ptr<Renderer>& renderer);

        void ReconstructMeshes(const std::shared_ptr<Renderer>& renderer);

        void TranslateRenderData(const std::shared_ptr<Renderer>& renderer);

        std::shared_ptr<Sandbox::GameObject> AddEmptyGameObject(const std::string& name, const Vector3& position);

        std::shared_ptr<Sandbox::GameObject> AddEmptyGameObject();

        void RemoveGameObject(const std::shared_ptr<Sandbox::GameObject>& gameObject);

        std::shared_ptr<Camera> FindFirstCamera();

        template <DerivedFromIComponent T>
        std::shared_ptr<T> RecursiveFindComponent(std::shared_ptr<GameObject>& gameObject);

    private:
        bool m_cleaned = false;

        Sandbox_Scene_GENERATED
    };


    template <DerivedFromIComponent T>
    std::shared_ptr<T> Scene::RecursiveFindComponent(std::shared_ptr<GameObject>& gameObject)
    {
        auto component = gameObject->GetComponent<T>();
        if (component != nullptr)
        {
            return component;
        }
        for (auto& child : gameObject->children)
        {
            component = RecursiveFindComponent<T>(child);
            if (component != nullptr)
            {
                return component;
            }
        }
        return nullptr;
    }
}  // namespace Sandbox NAMESPACE()

File_Scene_GENERATED
