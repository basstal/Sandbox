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
        std::shared_ptr<Meta> meta;
    };

    struct AssetTreeViewItem : public TreeViewItem
    {
    };

    class ContentBrowser : public TreeView
    {
    public:
        void Prepare() override;

        std::vector<std::shared_ptr<Sandbox::TreeViewItem>> ScanAndConstructContentBrowserTreeViewItems(const std::shared_ptr<Directory>& directory);

        void Tick(float deltaTime) override;

        void OnGui() override;

        intptr_t CreateLeafId(const std::shared_ptr<AssetFileTreeViewSource>& file);

        void ConstructImGuiTreeNodes(const std::shared_ptr<Sandbox::TreeViewSource>& target);

        void Cleanup() override;

    private:
        intptr_t m_singleClicked = -1;
        bool m_cleaned = false;
        std::set<intptr_t> m_selections;
    };
}
