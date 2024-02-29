#pragma once
#include <string>
#include <yaml-cpp/yaml.h>

#include "Infrastructures/ISerializable.hpp"
#include "Persistence/RendererPersistence.hpp"

/**
 * \brief 渲染器设置
 */
class RendererSettings : public ISerializable
{
public:
    /**
     * \brief 持久化数据
     */
    RendererPersistence persistence;

    /**
     * \brief 构造函数
     */
    RendererSettings();

    /**
     * \brief 析构函数
     */
    ~RendererSettings() override;

    /**
     * \brief 序列化
     */
    void Save() override;

    /**
     * \brief 反序列化
     */
    void Load() override;

    /**
     * \brief 获取序列化路径
     * \return
     */
    std::string GetSerializedPath() override;
};
