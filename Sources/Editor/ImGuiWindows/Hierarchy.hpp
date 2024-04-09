#pragma once
#include "Editor/ImGuiWidgets/TreeView.hpp"
#include "Engine/EntityComponent/Scene.hpp"
#include "Misc/Event.hpp"

namespace Sandbox
{
    class Inspector;

    struct HierarchyTreeViewSource : public TreeViewSource
    {
        std::shared_ptr<GameObject> gameObject;
    };

    struct HierarchyTreeViewItem : public TreeViewItem
    {
    };

    class Hierarchy : public TreeView
    {
    public:
        Hierarchy(const std::shared_ptr<Inspector>& inspector);

        std::vector<std::shared_ptr<Sandbox::TreeViewItem>> CreateTreeViewItems(const std::shared_ptr<Scene>& scene);

        std::vector<std::shared_ptr<Sandbox::TreeViewItem>> ChildGameObjects(const std::shared_ptr<GameObject>& gameObject);

        void Prepare() override;

        void OnGui() override;

        void                        Tick(float deltaTime) override;
        std::shared_ptr<GameObject> LeafIdToGameObject(intptr_t inPtr);

        void SetScene(const std::shared_ptr<Scene>& inScene);

        Event<std::shared_ptr<GameObject>> onTargetChanged;


    private:
        std::shared_ptr<Inspector> m_inspector;
    };
}  // namespace Sandbox
