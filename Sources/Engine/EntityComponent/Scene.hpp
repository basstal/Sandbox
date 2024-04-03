#pragma once
#include <memory>
#include "Generated/Scene.rfkh.h"
#include "Serialization/List.hpp"
#include "Serialization/ISerializable.hpp"
#include "GameObject.hpp"

namespace Sandbox NAMESPACE()
{
    // class GameObject;

    class CLASS() Scene : public ISerializable<Scene>
    {
    public:
        FIELD()
        List<SharedPtr<GameObject>> rootGameObjects;

        static std::shared_ptr<Scene> currentScene;

        static std::shared_ptr<Scene> GetCurrentScene();

        void Cleanup();

    private:
        bool m_cleaned = false;

        Sandbox_Scene_GENERATED

    };
}

File_Scene_GENERATED
