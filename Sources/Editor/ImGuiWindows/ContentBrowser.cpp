#include "ContentBrowser.hpp"

#include "imgui.h"
#include "FileSystem/Directory.hpp"
#include "FileSystem/File.hpp"

void Sandbox::ContentBrowser::Prepare()
{
    name = "ContentBrowser";
    TreeView::Prepare();
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
    Sandbox::TreeView::Tick(deltaTime);
}

void Sandbox::ContentBrowser::OnGui()
{
    TreeView::OnGui();
}


void Sandbox::ContentBrowser::Cleanup()
{
    TreeView::Cleanup();
}
