#pragma once
#include <memory>

#include "GameObject.hpp"
#include "Generated/Scene.rfkh.h"
#include "Serialization/ISerializable.hpp"
#include "Serialization/List.hpp"

namespace Sandbox NAMESPACE()
{
    struct Models;
    class Renderer;
    class Skybox;

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


    private:
        bool m_cleaned = false;

        Sandbox_Scene_GENERATED
    };
}  // namespace Sandbox NAMESPACE()

File_Scene_GENERATED
