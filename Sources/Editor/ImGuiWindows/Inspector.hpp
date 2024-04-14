#pragma once

#include "Editor/IImGuiWindow.hpp"
#include "Engine/EntityComponent/GameObject.hpp"
#include "Generated/Inspector.rfkh.h"

namespace Sandbox NAMESPACE()
{
    class CLASS() Inspector : public IImGuiWindow
    {
    public:
        Inspector();

        void Prepare() override;

        virtual void OnInspectorGui();

        void OnGui() override;

        void DrawComponentInspectors();

        void InspectTarget(std::shared_ptr<GameObject> inTarget);

        static std::map<std::string, std::shared_ptr<Sandbox::Inspector>> componentNameToInspector;

        template <DerivedFromIComponent T>
        static std::shared_ptr<Inspector> GetInspector();

        template <DerivedFromIComponent T>
        void DrawFieldsReflected(std::shared_ptr<T> instance);

        // TODO:记在这里太分散了，最好还是使用 hierarchy 一致的 target 记录
        std::shared_ptr<GameObject> target;
    protected:

        Sandbox_Inspector_GENERATED
    };

    template <DerivedFromIComponent T>
    std::shared_ptr<Inspector> Inspector::GetInspector()
    {
        std::string className = T::staticGetArchetype().getName();  // TODO:这里有个假设 派生自 IComponent 的类必须也派生自 ISerializable
        if (!componentNameToInspector.contains(className))
        {
            return nullptr;
        }
        return componentNameToInspector[className];
    }

    template <DerivedFromIComponent T>
    void Inspector::DrawFieldsReflected(std::shared_ptr<T> instance)
    {
        const rfk::Class& componentClass = T::staticGetArchetype();
        // 遍历每个字段
        componentClass.foreachField(
            [](const rfk::Field& field, void* userData)
            {
                // 获取字段类型信息
                const rfk::Type& fieldType   = field.getType();
                auto             instancePtr = static_cast<T*>(userData);

                // 检查并绘制整型字段
                if (fieldType.match(rfk::getType<int>()))
                {
                    int value = field.get<int, T>(*instancePtr);
                    if (ImGui::InputInt(field.getName(), &value))
                    {
                        field.set(*instancePtr, value);
                    }
                }
                // 检查并绘制浮点型字段
                else if (fieldType.match(rfk::getType<float>()))
                {
                    float value = field.get<float, T>(*instancePtr);
                    if (ImGui::InputFloat(field.getName(), &value))
                    {
                        field.set(*instancePtr, value);
                    }
                }
                // 检查并绘制布尔型字段
                else if (fieldType.match(rfk::getType<bool>()))
                {
                    bool value = field.get<bool, T>(*instancePtr);
                    if (ImGui::Checkbox(field.getName(), &value))
                    {
                        field.set(*instancePtr, value);
                    }
                }
                // 检查并绘制字符串字段
                // 注意: ImGui对字符串的处理比较特殊，通常需要一个可编辑的buffer
                // 这里只是简单示例，实际使用时可能需要动态分配buffer并在编辑后拷贝回去
                else if (fieldType.match(rfk::getType<char*>()))
                {
                    char* value = field.get<char*, T>(*instancePtr);
                    if (ImGui::InputText(field.getName(), value, 256))
                    {
                        field.set(*instancePtr, value);
                    }
                }
                // TODO:此处可以扩展更多类型的支持

                return true; // 继续遍历
            },instance.get());
    }
}  // namespace Sandbox NAMESPACE()

File_Inspector_GENERATED
