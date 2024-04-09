#include "pch.hpp"

#include "TreeView.hpp"

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
            TreeNodeClickEvent event(treeViewItem->source->name, leafId);
            // 派发事件
            OnTreeNodeDoubleClickDispatch(event);
        }

        if (ImGui::BeginDragDropSource())
        {
            // TODO:待完善
            ImGui::SetDragDropPayload("_TREENODE", NULL, 0);
            ImGui::Text("This is a drag and drop source");
            ImGui::EndDragDropSource();
        }
    }
    else if (target == root)  // root 不展示
    {
        for (auto& item : target->items)
        {
            ConstructImGuiTreeNodes(item);
        }
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

void Sandbox::TreeView::OnTreeNodeDoubleClickDispatch(TreeNodeClickEvent& clicked)
{
}
