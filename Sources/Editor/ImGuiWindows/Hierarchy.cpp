#include "pch.hpp"

#include "Hierarchy.hpp"

#include "Engine/EntityComponent/GameObject.hpp"
#include "Engine/EntityComponent/Scene.hpp"

void Sandbox::Hierarchy::Cleanup()
{
    TreeView::Cleanup();
}

std::vector<std::shared_ptr<Sandbox::TreeViewItem>> Sandbox::Hierarchy::CreateTreeViewItems(const std::shared_ptr<Scene>& scene)
{
    std::vector<std::shared_ptr<TreeViewItem>> result;
    for (auto& gameObject : scene->rootGameObjects)
    {
        auto item = std::make_shared<HierarchyTreeViewItem>();
        auto source = std::make_shared<HierarchyTreeViewSource>();
        source->name = gameObject->name;
        source->items = ChildGameObjects(gameObject);
        item->source = source;
        result.push_back(item);
    }
    return result;
}

std::vector<std::shared_ptr<Sandbox::TreeViewItem>> Sandbox::Hierarchy::ChildGameObjects(const std::shared_ptr<GameObject>& gameObject)
{
    std::vector<std::shared_ptr<TreeViewItem>> result;
    for (auto& childGameObject : gameObject->children)
    {
        auto item = std::make_shared<HierarchyTreeViewItem>();
        auto source = std::make_shared<HierarchyTreeViewSource>();
        source->name = childGameObject->name;
        source->items = ChildGameObjects(childGameObject);
        item->source = source;
        result.push_back(item);
    }
    return result;
}

void Sandbox::Hierarchy::Prepare()
{
    TreeView::Prepare();
    auto hierarchyRoot = std::make_shared<HierarchyTreeViewSource>();
    hierarchyRoot->name = "HierarchyRoot";
    hierarchyRoot->items = CreateTreeViewItems(Scene::GetCurrentScene());
}

void Sandbox::Hierarchy::OnGui()
{
    TreeView::OnGui();
}

void Sandbox::Hierarchy::Tick(float deltaTime)
{
    TreeView::Tick(deltaTime);
}
