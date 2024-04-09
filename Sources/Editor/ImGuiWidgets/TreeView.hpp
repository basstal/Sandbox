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

    // 示例事件类型
    struct TreeNodeClickEvent
    {
        std::string nodeName;
        intptr_t    nodeId;

        TreeNodeClickEvent(const std::string& name, intptr_t id) : nodeName(name), nodeId(id) {}
    };

    class TreeView : public IImGuiWindow
    {
    public:
        std::shared_ptr<TreeViewItem> root;

        void Prepare() override;

        void OnGui() override;

        void Tick(float deltaTime) override;

        void Cleanup() override;

        virtual intptr_t GetLeafId(const std::shared_ptr<TreeViewItem>& source);

        std::shared_ptr<Sandbox::TreeViewItem> LeafIdToSharedPtr(intptr_t id);

        virtual void ConstructImGuiTreeNodes(const std::shared_ptr<Sandbox::TreeViewItem>& target);

        virtual void OnTreeNodeDoubleClickDispatch(TreeNodeClickEvent&);

    protected:
        std::map<intptr_t, std::shared_ptr<TreeViewItem>> m_idToItem;
        bool                                              m_cleaned       = false;
        intptr_t                                          m_singleClicked = -1;
        std::set<intptr_t>                                m_selections;
    };
}  // namespace Sandbox
