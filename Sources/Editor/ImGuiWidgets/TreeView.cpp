#include "TreeView.hpp"

void Sandbox::TreeView::Prepare()
{
}

void Sandbox::TreeView::OnGui()
{
}

void Sandbox::TreeView::Tick(float deltaTime)
{
    IImGuiWindow::Tick(deltaTime);
}

void Sandbox::TreeView::Cleanup()
{
    if (m_cleaned)
    {
        return;
    }
    m_cleaned = true;
}
