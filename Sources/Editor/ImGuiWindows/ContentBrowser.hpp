#pragma once
#include <memory>
#include <set>

#include "Editor/IImGuiWindow.hpp"
#include "Editor/ImGuiWidgets/TreeView.hpp"

namespace Sandbox
{
    class File;
    class Directory;

    struct Meta
    {
    };

    struct AssetFileTreeViewSource : public TreeViewSource
    {
        std::shared_ptr<File> file;
        std::shared_ptr<Meta> meta;
    };

    struct AssetDirectoryTreeViewSource : public TreeViewSource
    {
        std::shared_ptr<Directory> directory;
        std::shared_ptr<Meta>      meta;
    };

    struct AssetTreeViewItem : public TreeViewItem
    {
    };

    class ContentBrowser : public TreeView
    {
    public:
        ContentBrowser();

        void Prepare() override;

        std::vector<std::shared_ptr<Sandbox::TreeViewItem>> ScanAndConstructContentBrowserTreeViewItems(const std::shared_ptr<Directory>& directory);

        void Tick(float deltaTime) override;

        void OnGui() override;

        void Cleanup() override;

        void OnTreeNodeDoubleClickDispatch(TreeNodeClickEvent&) override;
    };
}  // namespace Sandbox
