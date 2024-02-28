#pragma once
#include <any>
#include <map>
#include <memory>
#include <typeindex>
#include <unordered_map>

#include "FileSystem/Logger.hpp"

/**
 * \brief 单例管理器
 * 单例之间应尽量避免相互依赖
 */
class SingletonOrganizer
{
    /**
     * \brief 单例容器
     */
    static std::map<int32_t, std::any> m_singletons;

    /**
     * \brief 类型索引到类型 ID 的映射
     */
    static std::unordered_map<std::type_index, int32_t> m_typeIndexToTypeId;

    /**
     * \brief 下一个类型 ID
     */
    static int32_t m_nextTypeId;

public:
    /**
     * \brief 注册单例
     * \tparam T 注册的单例类型
     */
    template <typename T>
    static void Register();

    /**
     * \brief 获取单例
     * \tparam T 获取的单例类型
     * \return 获取的单例
     */
    template <typename T>
    static std::shared_ptr<T> Get();

    /**
     * \brief 获取类型的 ID
     * \tparam T 获取 ID 的类型
     * \return 类型 ID
     */
    template <typename T>
    static int32_t GetTypeId();
};

template <typename T>
void SingletonOrganizer::Register()
{
    int32_t typeId = GetTypeId<T>();
    std::shared_ptr<T> ptr = std::make_shared<T>();
    m_singletons.emplace(typeId, ptr);
}

template <typename T>
std::shared_ptr<T> SingletonOrganizer::Get()
{
    try
    {
        return std::any_cast<std::shared_ptr<T>>(m_singletons[GetTypeId<T>()]);
    }
    catch (const std::bad_any_cast&)
    {
        Logger::Fatal("SingletonOrganizer::Get: Singleton not found");
    }
    return nullptr;
}

template <typename T>
int32_t SingletonOrganizer::GetTypeId()
{
    const std::type_index typeIndex(typeid(T));
    if (!m_typeIndexToTypeId.contains(typeIndex))
    {
        m_typeIndexToTypeId[typeIndex] = m_nextTypeId++;
    }
    return m_typeIndexToTypeId[typeIndex];
}
