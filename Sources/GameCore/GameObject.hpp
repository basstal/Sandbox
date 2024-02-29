#pragma once
#include <memory>
#include <vector>
#include <GameCore/IComponent.hpp>

class Transform;


/**
 * \brief 游戏对象类
 */
class GameObject
{
    /**
     * \brief 是否已经清理
     */
    bool m_cleaned = false;

public:
    /**
     * \brief 父对象
     */
    std::shared_ptr<GameObject> parent;
    /**
     * \brief 变换信息
     */
    std::shared_ptr<Transform> transform;
    /**
     * \brief 组件列表
     */
    std::vector<std::shared_ptr<IComponent>> components;

    /**
     * \brief 构造函数
     */
    GameObject();

    /**
     * \brief 析构函数
     */
    ~GameObject();

    /**
     * \brief 清理资源
     */
    void Cleanup();

    /**
     * \brief 添加组件
     * \param component 组件指针
     */
    void AddComponent(std::shared_ptr<IComponent> component);

    /**
     * \brief 获得组件
     * \tparam T 组件类型
     * \return 组件指针
     */
    template <DerivedFromIComponent T>
    std::shared_ptr<T> GetComponent();
};


template <DerivedFromIComponent T>
std::shared_ptr<T> GameObject::GetComponent()
{
    for (auto& component : components)
    {
        if (std::dynamic_pointer_cast<T>(component))
        {
            return std::dynamic_pointer_cast<T>(component);
        }
    }
    return nullptr;
}
