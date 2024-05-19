#include "pch.hpp"

#include "TreeView.hpp"

#include "Misc/Debug.hpp"

bool Sandbox::TreeViewItem::IsLeaf() { return items.size() == 0; }

void Sandbox::TreeView::Prepare() { IImGuiWindow::Prepare(); }

void Sandbox::TreeView::OnGui()
{
    m_singleClicked = -1;
    ConstructImGuiTreeNodes(root);
    if (m_singleClicked != -1)
    {
        // Update selection state
        // (process outside of tree loop to avoid visual inconsistencies during the clicking frame)
        if (ImGui::GetIO().KeyCtrl)
        {
            m_selections.emplace(m_singleClicked);  // CTRL+click to toggle
        }
        else
        {
            // if (!(selection_mask & (1 << node_clicked))) // Depending on selection behavior you want, may want to preserve selection when clicking on item that is part of the
            // selection
            m_selections.clear();
            m_selections.emplace(m_singleClicked);  // Click to single-select
        }
    }
}

void Sandbox::TreeView::Tick(float deltaTime) { IImGuiWindow::Tick(deltaTime); }

void Sandbox::TreeView::Cleanup()
{
    if (m_cleaned)
    {
        return;
    }
    IImGuiWindow::Cleanup();
    m_cleaned = true;
}

intptr_t Sandbox::TreeView::GetLeafId(const std::shared_ptr<TreeViewItem>& item)
{
    if (item == nullptr)
    {
        return -1;
    }
    auto id        = reinterpret_cast<intptr_t>(item.get());
    m_idToItem[id] = item;
    return id;
}


std::shared_ptr<Sandbox::TreeViewItem> Sandbox::TreeView::LeafIdToSharedPtr(intptr_t id) { return m_idToItem[id]; }

void Sandbox::TreeView::ConstructImGuiTreeNodes(const std::shared_ptr<Sandbox::TreeViewItem>& target)
{
    if (target == nullptr)
    {
        return;
    }
    if (target == root)  // root 不展示，即使它是叶节点
    {
        for (auto& item : target->items)
        {
            ConstructImGuiTreeNodes(item);
        }
        return;
    }
    auto treeViewItem = std::dynamic_pointer_cast<TreeViewItem>(target);
    if (treeViewItem->IsLeaf())
    {
        static ImGuiTreeNodeFlags baseFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth |
            ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;  // ImGuiTreeNodeFlags_Bullet;
        auto leafId    = GetLeafId(treeViewItem);
        auto nodeFlags = m_selections.contains(leafId) ? baseFlags | ImGuiTreeNodeFlags_Selected : baseFlags;
        ImGui::TreeNodeEx(reinterpret_cast<void*>(leafId), nodeFlags,
                          treeViewItem->source->name.c_str());  // NOLINT(clang-diagnostic-format-security, performance-no-int-to-ptr)
        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
        {
            m_singleClicked = leafId;
        }

        // 检测到双击事件
        if (ImGui::IsItemClicked(0) && ImGui::IsMouseDoubleClicked(0))
        {
            // 构建事件信息
            TreeNodeClickEvent event(treeViewItem->source->name, leafId, LeftMouseDouble);
            // 派发事件
            OnTreeNodeClickDispatch(event);
        }

        // 检测鼠标左键点击事件
        if (ImGui::IsItemClicked(0))
        {
            // 构建事件信息
            TreeNodeClickEvent event(treeViewItem->source->name, leafId, LeftMouse);
            // 派发事件
            OnTreeNodeClickDispatch(event);
        }

        if (ImGui::IsItemClicked(1))
        {
            // 构建事件信息
            TreeNodeClickEvent event(treeViewItem->source->name, leafId, RightMouse);
            // 派发事件
            OnTreeNodeClickDispatch(event);
        }

        DragAndDrop(treeViewItem);
    }
    else if (ImGui::TreeNode(target->source->name.c_str()))
    {
        for (auto& item : target->items)
        {
            ConstructImGuiTreeNodes(item);
        }
        ImGui::TreePop();
    }
}

void Sandbox::TreeView::DragAndDrop(std::shared_ptr<TreeViewItem>& inTreeViewItem)
{
    // if (ImGui::BeginDragDropSource())
    // {
    //     ImGui::SetDragDropPayload("_TREENODE", &inTreeViewItem, sizeof(inTreeViewItem));
    //     // ImGui::Text("Dragging shared object with value: %d", *treeViewItem);
    //     ImGui::EndDragDropSource();
    // }
}

void Sandbox::TreeView::OnTreeNodeClickDispatch(TreeNodeClickEvent& treeNodeClickEvent)
{
    if (treeNodeClickEvent.clickType == RightMouse)
    {
        m_singleClicked = treeNodeClickEvent.nodeId;
    }
}

