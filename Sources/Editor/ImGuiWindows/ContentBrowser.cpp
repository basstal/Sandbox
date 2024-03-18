#include "ContentBrowser.hpp"

#include "imgui.h"
#include "FileSystem/Directory.hpp"
#include "FileSystem/File.hpp"

void Sandbox::ContentBrowser::Prepare()
{
    name = "ContentBrowser";
    auto assignRoot = std::make_shared<AssetDirectoryTreeViewSource>();
    auto rootDirectory = Directory::GetAssetsDirectory();
    assignRoot->name = "Root";
    assignRoot->directory = std::make_shared<Directory>(rootDirectory);
    assignRoot->items = ScanAndConstructContentBrowserTreeViewItems(assignRoot->directory);
    assignRoot->meta = std::make_shared<Meta>(); // TODO:文件夹的 meta 信息要如何保留，涉及到一些序列化和资源导入的问题
    root = assignRoot;
}


std::vector<std::shared_ptr<Sandbox::TreeViewItem>> Sandbox::ContentBrowser::ScanAndConstructContentBrowserTreeViewItems(const std::shared_ptr<Directory>& directory)
{
    std::vector<std::shared_ptr<TreeViewItem>> items;
    for (const auto& file : directory->GetFiles())
    {
        auto container = std::make_shared<AssetTreeViewItem>();
        auto assignItem = std::make_shared<AssetFileTreeViewSource>();
        assignItem->name = file.GetName();
        assignItem->file = std::make_shared<File>(file);
        assignItem->items = {};
        container->source = assignItem;
        items.push_back(container);
    }
    // TODO:这里假设了文件夹不包含回环引用，如果包含如超链接或映像等可能导致回环的文件系统，需要特殊处理
    for (const auto& subDirectory : directory->GetDirectories())
    {
        auto container = std::make_shared<AssetTreeViewItem>();
        auto assignItem = std::make_shared<AssetDirectoryTreeViewSource>();
        assignItem->name = subDirectory.GetName();
        assignItem->directory = std::make_shared<Directory>(subDirectory);
        assignItem->items = ScanAndConstructContentBrowserTreeViewItems(assignItem->directory);
        container->source = assignItem;
        items.push_back(container);
    }
    return items;
}

void Sandbox::ContentBrowser::Tick(float deltaTime)
{
    IImGuiWindow::Tick(deltaTime);
}

void Sandbox::ContentBrowser::OnGui()
{
    m_singleClicked = -1;
    ConstructImGuiTreeNodes(root);
    if (m_singleClicked != -1)
    {
        // Update selection state
        // (process outside of tree loop to avoid visual inconsistencies during the clicking frame)
        if (ImGui::GetIO().KeyCtrl)
        {
            m_selections.emplace(m_singleClicked); // CTRL+click to toggle
        }
        else
        {
            //if (!(selection_mask & (1 << node_clicked))) // Depending on selection behavior you want, may want to preserve selection when clicking on item that is part of the selection
            m_selections.clear();
            m_selections.emplace(m_singleClicked); // Click to single-select
        }
    }
}

intptr_t Sandbox::ContentBrowser::CreateLeafId(const std::shared_ptr<AssetFileTreeViewSource>& file)
{
    if (file == nullptr)
    {
        return -1;
    }
    return reinterpret_cast<intptr_t>(file.get());
}

void Sandbox::ContentBrowser::ConstructImGuiTreeNodes(const std::shared_ptr<Sandbox::TreeViewSource>& target)
{
    if (target == nullptr)
    {
        return;
    }
    auto file = std::dynamic_pointer_cast<AssetFileTreeViewSource>(target);
    if (file)
    {
        static ImGuiTreeNodeFlags baseFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Leaf |
            ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet;
        auto leafId = CreateLeafId(file);
        auto nodeFlags = m_selections.contains(leafId) ? baseFlags | ImGuiTreeNodeFlags_Selected : baseFlags;
        ImGui::TreeNodeEx(reinterpret_cast<void*>(leafId), nodeFlags, file->name.c_str()); // NOLINT(clang-diagnostic-format-security, performance-no-int-to-ptr)
        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
        {
            m_singleClicked = leafId;
        }
        if (ImGui::BeginDragDropSource())
        {
            // TODO:待完善
            ImGui::SetDragDropPayload("_TREENODE", NULL, 0);
            ImGui::Text("This is a drag and drop source");
            ImGui::EndDragDropSource();
        }
    }
    else if (target == root) // root 不展示
    {
        for (auto& item : target->items)
        {
            ConstructImGuiTreeNodes(item->source);
        }
    }
    else if (ImGui::TreeNode(target->name.c_str()))
    {
        for (auto& item : target->items)
        {
            ConstructImGuiTreeNodes(item->source);
        }
        ImGui::TreePop();
    }
}

void Sandbox::ContentBrowser::Cleanup()
{
    if (m_cleaned)
    {
        return;
    }
    m_cleaned = true;
}
