#pragma once
#include <memory>

#include "GameObject.hpp"
#include "Generated/Scene.rfkh.h"
#include "Serialization/ISerializable.hpp"
#include "Serialization/List.hpp"

namespace Sandbox NAMESPACE()
{
    // class GameObject;

    class CLASS() Scene : public ISerializable//<Scene>
    {
    public:
        FIELD()
        List<SharedPtr<GameObject>> rootGameObjects;

        static std::shared_ptr<Scene> currentScene;

        static std::shared_ptr<Scene> GetCurrentScene();

        void Cleanup();

        void Tick();
    private:
        bool m_cleaned = false;

        Sandbox_Scene_GENERATED
    };
}  // namespace Sandbox NAMESPACE()

File_Scene_GENERATED
