#pragma once
#include "Editor/ImGuiWidgets/TreeView.hpp"
#include "Engine/EntityComponent/Scene.hpp"

namespace Sandbox
{
    struct HierarchyTreeViewSource : public TreeViewSource
    {
    };

    struct HierarchyTreeViewItem : public TreeViewItem
    {
    };

    class Hierarchy : public TreeView
    {
    public:
        void Cleanup() override;

        std::vector<std::shared_ptr<Sandbox::TreeViewItem>> CreateTreeViewItems(const std::shared_ptr<Scene>& scene);

        std::vector<std::shared_ptr<Sandbox::TreeViewItem>> ChildGameObjects(const std::shared_ptr<GameObject>& gameObject);

        void Prepare() override;

        void OnGui() override;

        void Tick(float deltaTime) override;
    };
}
