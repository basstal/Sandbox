#pragma once
#include "yaml-cpp/node/node.h"

/**
 * \brief 序列化接口
 */
class ISerializable
{
public:
    /**
     * \brief 析构函数
     */
    virtual ~ISerializable() = default;

    /**
     * \brief 获取序列化路径
     * \return
     */
    virtual std::string GetSerializedPath() = 0;

    /**
     * \brief 序列化
     */
    virtual void Save() = 0;

    /**
     * \brief 反序列化
     */
    virtual void Load() = 0;
};
