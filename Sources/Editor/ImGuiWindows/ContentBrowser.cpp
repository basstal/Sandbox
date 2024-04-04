#include "pch.hpp"

#include "ContentBrowser.hpp"

#include "FileSystem/Directory.hpp"
#include "FileSystem/File.hpp"

Sandbox::ContentBrowser::ContentBrowser() { name = "ContentBrowser"; }

void Sandbox::ContentBrowser::Prepare()
{
    TreeView::Prepare();
    auto item          = std::make_shared<AssetTreeViewItem>();
    auto source        = std::make_shared<AssetDirectoryTreeViewSource>();
    auto rootDirectory = Directory::GetAssetsDirectory();
    source->name       = "Root";
    source->directory  = std::make_shared<Directory>(rootDirectory);
    source->meta       = std::make_shared<Meta>();  // TODO:文件夹的 meta 信息要如何保留，涉及到一些序列化和资源导入的问题
    item->items        = ScanAndConstructContentBrowserTreeViewItems(source->directory);
    item->source       = source;
    root               = item;
}

std::vector<std::shared_ptr<Sandbox::TreeViewItem>> Sandbox::ContentBrowser::ScanAndConstructContentBrowserTreeViewItems(const std::shared_ptr<Directory>& directory)
{
    std::vector<std::shared_ptr<TreeViewItem>> items;
    for (const auto& file : directory->GetFiles())
    {
        auto container    = std::make_shared<AssetTreeViewItem>();
        auto source       = std::make_shared<AssetFileTreeViewSource>();
        source->name      = file.GetName();
        source->file      = std::make_shared<File>(file);
        container->items  = {};
        container->source = source;
        items.push_back(container);
    }
    // TODO:这里假设了文件夹不包含回环引用，如果包含如超链接或映像等可能导致回环的文件系统，需要特殊处理
    for (const auto& subDirectory : directory->GetDirectories())
    {
        auto container    = std::make_shared<AssetTreeViewItem>();
        auto source       = std::make_shared<AssetDirectoryTreeViewSource>();
        source->name      = subDirectory.GetName();
        source->directory = std::make_shared<Directory>(subDirectory);
        container->items  = ScanAndConstructContentBrowserTreeViewItems(source->directory);
        container->source = source;
        items.push_back(container);
    }
    return items;
}

void Sandbox::ContentBrowser::Tick(float deltaTime) { Sandbox::TreeView::Tick(deltaTime); }

void Sandbox::ContentBrowser::OnGui() { TreeView::OnGui(); }

void Sandbox::ContentBrowser::Cleanup() { TreeView::Cleanup(); }
