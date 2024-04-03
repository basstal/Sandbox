#include "TransformInspector.hpp"

Sandbox::TransformInspector::TransformInspector() { name = "TransformInspector"; }

void Sandbox::TransformInspector::OnGui()
{
    // 创建一个子区域/子窗口
    ImGui::BeginChild(name.c_str());
    ImGui::Text("This is a child area");
    // 在这里添加更多的ImGui绘制调用，它们会在子区域中渲染
    ImGui::EndChild(); // 结束子区域/子窗口
}
