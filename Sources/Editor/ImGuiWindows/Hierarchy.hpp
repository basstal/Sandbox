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
        void ShowContextMenu();

        void Tick(float deltaTime) override;

        std::shared_ptr<GameObject> LeafIdToGameObject(intptr_t inPtr);
        intptr_t                    FindGameObjectInTree(std::shared_ptr<Sandbox::TreeViewItem> target, std::shared_ptr<Sandbox::GameObject>& gameObject);
        intptr_t                    GameObjectToLeafId(std::shared_ptr<Sandbox::GameObject>& gameObject);

        void SetScene(const std::shared_ptr<Scene>& inScene);
        void GameObjectToSelection(std::shared_ptr<GameObject> inTarget);
        void DragAndDrop(std::shared_ptr<TreeViewItem>& inTreeViewItem) override;

    private:
        std::shared_ptr<Inspector>  m_inspector;
        bool                        m_createNewGameObject;
        bool                        m_deleteSelectGameObject;
        std::shared_ptr<GameObject> m_copiedGameObject;
        bool                        m_createPasteGameObject;
    };
}  // namespace Sandbox
