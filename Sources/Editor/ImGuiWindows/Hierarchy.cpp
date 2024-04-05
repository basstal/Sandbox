#include "pch.hpp"

#include "Hierarchy.hpp"

#include "Engine/EntityComponent/GameObject.hpp"
#include "Engine/EntityComponent/Scene.hpp"
#include "Inspector.hpp"

Sandbox::Hierarchy::Hierarchy(const std::shared_ptr<Inspector>& inspector)
{
    name        = "Hierarchy";
    m_inspector = inspector;
}
std::vector<std::shared_ptr<Sandbox::TreeViewItem>> Sandbox::Hierarchy::CreateTreeViewItems(const std::shared_ptr<Scene>& scene)
{
    std::vector<std::shared_ptr<TreeViewItem>> result;
    for (auto& gameObject : scene->rootGameObjects)
    {
        auto item   = std::make_shared<HierarchyTreeViewItem>();
        auto source = std::make_shared<HierarchyTreeViewSource>();
        // TODO: restore gameObject as ptr
        LOGD_OLD("gameObject.name: {}", gameObject->name.ToStdString())
        source->name       = gameObject->name.ToStdString();
        source->gameObject = gameObject.ToStdSharedPtr();
        item->items        = ChildGameObjects(gameObject);
        item->source       = source;
        result.push_back(item);
    }
    return result;
}

std::vector<std::shared_ptr<Sandbox::TreeViewItem>> Sandbox::Hierarchy::ChildGameObjects(const std::shared_ptr<GameObject>& gameObject)
{
    std::vector<std::shared_ptr<TreeViewItem>> result;
    for (auto& childGameObject : gameObject->children)
    {
        auto item          = std::make_shared<HierarchyTreeViewItem>();
        auto source        = std::make_shared<HierarchyTreeViewSource>();
        source->name       = childGameObject->name.ToStdString();
        source->gameObject = childGameObject;
        item->items        = ChildGameObjects(childGameObject);
        item->source       = source;
        result.push_back(item);
    }
    return result;
}

void Sandbox::Hierarchy::Prepare()
{
    TreeView::Prepare();
    SetScene(Scene::GetCurrentScene());
}

void Sandbox::Hierarchy::OnGui() { TreeView::OnGui(); }

void Sandbox::Hierarchy::Tick(float deltaTime)
{
    TreeView::Tick(deltaTime);
    if (m_selections.size() > 0)
    {
        auto gameObject = LeafIdToGameObject(*m_selections.cbegin());
        onTargetChanged.Trigger(gameObject);
    }
}


std::shared_ptr<Sandbox::GameObject> Sandbox::Hierarchy::LeafIdToGameObject(intptr_t inPtr)
{
    HierarchyTreeViewItem* item = reinterpret_cast<HierarchyTreeViewItem*>(inPtr);
    return std::dynamic_pointer_cast<HierarchyTreeViewSource>(item->source)->gameObject;
}
void Sandbox::Hierarchy::SetScene(const std::shared_ptr<Scene>& inScene)
{
    if (inScene == nullptr)
    {
        return;
    }
    auto hierarchyRoot  = std::make_shared<HierarchyTreeViewSource>();
    auto container      = std::make_shared<HierarchyTreeViewItem>();
    hierarchyRoot->name = "HierarchyRoot";
    container->source   = hierarchyRoot;
    container->items    = CreateTreeViewItems(inScene);
    root                = container;
}
