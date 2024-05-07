#include "pch.hpp"

#include "Hierarchy.hpp"

#include "Editor/ImGuiRenderer.hpp"
#include "Engine/EntityComponent/GameObject.hpp"
#include "Engine/EntityComponent/Scene.hpp"
#include "Inspector.hpp"
#include "Misc/Debug.hpp"

Sandbox::Hierarchy::Hierarchy(const std::shared_ptr<Inspector>& inspector)
{
    name                     = "Hierarchy";
    m_inspector              = inspector;
    m_createNewGameObject    = false;
    m_deleteSelectGameObject = false;
    Scene::onSceneChange.Bind(
        [this](const std::shared_ptr<Scene>& inScene)
        {
            inScene->onHierarchyChanged.Bind(
                [this]
                {
                    // TODO:这里可以简化，目前粗暴重构整个 Hierarchy
                    SetScene(Scene::currentScene);
                });
            SetScene(inScene);
        });
    ImGuiRenderer::Instance->onTargetChanged.BindMember<Hierarchy, &Hierarchy::GameObjectToSelection>(this);
}
std::vector<std::shared_ptr<Sandbox::TreeViewItem>> Sandbox::Hierarchy::CreateTreeViewItems(const std::shared_ptr<Scene>& scene)
{
    std::vector<std::shared_ptr<TreeViewItem>> result;
    for (auto& gameObject : scene->rootGameObjects)
    {
        auto item   = std::make_shared<HierarchyTreeViewItem>();
        auto source = std::make_shared<HierarchyTreeViewSource>();
        // TODO: restore gameObject as ptr
        LOGD("Editor", "gameObject.name: {}", gameObject->name.ToStdString())
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

// TODO:temp code
void ShowExampleMenuFile()
{
    // 菜单项
    if (ImGui::MenuItem("New"))
    {
        // 当'New'被选中时执行的代码
    }
    if (ImGui::MenuItem("Open", "Ctrl+O"))
    {
        // 当'Open'被选中时执行的代码
    }
    // 更多菜单项...
    ImGui::Separator();
    if (ImGui::MenuItem("Quit", "Alt+F4"))
    {
        // 当'Quit'被选中时执行的代码
    }
}

void Sandbox::Hierarchy::OnGui()
{
    std::shared_ptr<GameObject> createdGameObject = nullptr;
    if (m_createNewGameObject)
    {
        createdGameObject = Scene::currentScene->AddEmptyGameObject();
    }
    if (m_deleteSelectGameObject)
    {
        for (auto& selection : m_selections)
        {
            auto gameObject = LeafIdToGameObject(selection);
            Scene::currentScene->RemoveGameObject(gameObject);
        }
        ImGuiRenderer::Instance->onTargetChanged.Trigger(nullptr);
    }
    m_deleteSelectGameObject = false;
    TreeView::OnGui();
    if (m_createNewGameObject)
    {
        m_selections.emplace(GameObjectToLeafId(createdGameObject));
        ImGuiRenderer::Instance->onTargetChanged.Trigger(createdGameObject);
    }
    m_createNewGameObject = false;

    if (m_singleClicked != -1 && !m_selections.empty())
    {
        auto gameObject = LeafIdToGameObject(*m_selections.cbegin());
        ImGuiRenderer::Instance->onTargetChanged.Trigger(gameObject);
    }

    // 如果窗口是右键点击，弹出上下文菜单
    if (ImGui::BeginPopupContextWindow())
    {
        if (ImGui::MenuItem("New GameObject"))
        {
            m_createNewGameObject = true;
        }
        if (ImGui::MenuItem("Delete"))
        {
            m_deleteSelectGameObject = true;
        }
        // 也可以复用其他的菜单
        // ShowExampleMenuFile();
        ImGui::EndPopup();
    }
}

void Sandbox::Hierarchy::Tick(float deltaTime) { TreeView::Tick(deltaTime); }


std::shared_ptr<Sandbox::GameObject> Sandbox::Hierarchy::LeafIdToGameObject(intptr_t inPtr)
{
    HierarchyTreeViewItem* item = reinterpret_cast<HierarchyTreeViewItem*>(inPtr);
    return std::dynamic_pointer_cast<HierarchyTreeViewSource>(item->source)->gameObject;
}


intptr_t Sandbox::Hierarchy::FindGameObjectInTree(std::shared_ptr<Sandbox::TreeViewItem> target, std::shared_ptr<Sandbox::GameObject>& gameObject)
{
    if (std::dynamic_pointer_cast<Sandbox::HierarchyTreeViewSource>(target->source)->gameObject == gameObject)
    {
        return GetLeafId(target);
    }
    if (target->IsLeaf())
    {
        return -1;
    }
    for (auto& item : target->items)
    {
        auto leafId = FindGameObjectInTree(item, gameObject);
        if (leafId != -1)
        {
            return leafId;
        }
    }
    return -1;
}

intptr_t Sandbox::Hierarchy::GameObjectToLeafId(std::shared_ptr<Sandbox::GameObject>& gameObject) { return FindGameObjectInTree(root, gameObject); }


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


void Sandbox::Hierarchy::GameObjectToSelection(std::shared_ptr<GameObject> inTarget)
{
    m_selections.clear();
    if (inTarget == nullptr)
    {
        return;
    }
    auto target = inTarget;
    auto items  = root->items;
    for (auto& item : items)
    {
        auto source = std::dynamic_pointer_cast<HierarchyTreeViewSource>(item->source);
        if (source->gameObject == target)
        {
            m_selections.emplace(reinterpret_cast<intptr_t>(item.get()));
            return;
        }
    }
    LOGF("Editor", "GameObject not found in Hierarchy tree!\n{}", GetCallStack())
    // TODO:处理子节点
    // for (auto& item : items)
    // {
    //     auto source = std::dynamic_pointer_cast<HierarchyTreeViewSource>(item->source);
    //     if (source->gameObject->children.size() > 0)
    //     {
    //         for (auto& child : source->gameObject->children)
    //         {
    //             if (child == target)
    //             {
    //                 m_selections.clear();
    //                 m_selections.emplace(reinterpret_cast<intptr_t>(item.get()));
    //                 return;
    //             }
    //         }
    //     }
    // }
}