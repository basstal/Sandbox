#pragma once
#include <memory>
#include <string>
#include <vector>

#include "Editor/IImGuiWindow.hpp"

namespace Sandbox
{
    struct TreeViewItem;

    struct TreeViewSource
    {
        std::string name;
        std::vector<std::shared_ptr<TreeViewItem>> items;

        virtual ~TreeViewSource() = default;
    };

    struct TreeViewItem
    {
        std::shared_ptr<TreeViewSource> source;

        virtual ~TreeViewItem() = default;
    };

    class TreeView : public IImGuiWindow
    {
    public:
        std::shared_ptr<TreeViewSource> root;

        void Prepare() override;

        void OnGui() override;

        void Tick(float deltaTime) override;

        void Cleanup() override;

    private:
        bool m_cleaned = false;
    };
}
