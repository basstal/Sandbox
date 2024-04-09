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

    class CLASS() Scene : public ISerializable  //<Scene>
    {
    public:
        FIELD()
        List<SharedPtr<GameObject>> rootGameObjects;

        static std::shared_ptr<Scene> currentScene;

        static std::shared_ptr<Scene> GetCurrentScene();

        void Cleanup();

        void Tick(const std::shared_ptr<Renderer>& renderer);

        static std::shared_ptr<Scene> LoadScene(std::shared_ptr<File> sceneFile);
        static void                   NewScene();


        std::vector<std::shared_ptr<Models>> models;

        static Event<const std::shared_ptr<Scene>&> onSceneChange;

    private:
        bool m_cleaned = false;

        Sandbox_Scene_GENERATED
    };
}  // namespace Sandbox NAMESPACE()

File_Scene_GENERATED
