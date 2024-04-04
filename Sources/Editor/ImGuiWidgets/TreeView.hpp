#pragma once
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "Editor/IImGuiWindow.hpp"

namespace Sandbox
{
    struct TreeViewItem;

    struct TreeViewSource
    {
        std::string name;

        virtual ~TreeViewSource() = default;
    };

    struct TreeViewItem
    {
        std::shared_ptr<TreeViewSource>            source;
        std::vector<std::shared_ptr<TreeViewItem>> items;

        bool IsLeaf();

        virtual ~TreeViewItem() = default;
    };

    class TreeView : public IImGuiWindow
    {
    public:
        std::shared_ptr<TreeViewItem> root;

        void Prepare() override;

        void OnGui() override;

        void Tick(float deltaTime) override;

        void Cleanup() override;

        virtual intptr_t CreateLeafId(const std::shared_ptr<TreeViewItem>& source);

        virtual void ConstructImGuiTreeNodes(const std::shared_ptr<Sandbox::TreeViewItem>& target);

    protected:
        bool               m_cleaned       = false;
        intptr_t           m_singleClicked = -1;
        std::set<intptr_t> m_selections;
    };
}  // namespace Sandbox
