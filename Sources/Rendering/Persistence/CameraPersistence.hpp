#pragma once
#include <boost/hana.hpp>
#include <yaml-cpp/yaml.h>

#include "Supports/YamlGlmConverter.hpp"

/**
 * \brief 相机持久化数据
 */
struct CameraPersistence
{
    /**
     * \brief 位置
     */
    glm::vec3 position = glm::vec3(0.0f);
    // Euler Angles
    /**
     * \brief X 轴旋转角度
     */
    float rotationX = 0.0f;
    /**
     * \brief Z 轴旋转角度
     */
    float rotationZ = 0.0f;
    /**
     * \brief 视野
     */
    float fieldOfView = 45.0f;
    /**
     * \brief 近平面
     */
    float nearPlane = 0.1f;
    /**
     * \brief 远平面
     */
    float farPlane = 100.0f;
    /**
     * \brief 纵横比
     */
    float aspectRatio = 1.0f;
};

BOOST_HANA_ADAPT_STRUCT(CameraPersistence, position, rotationX, rotationZ);

namespace YAML
{
    template <>
    struct convert<CameraPersistence>
    {
        static Node encode(const CameraPersistence& rhs)
        {
            Node node;
            node["position"] = rhs.position;
            node["rotationX"] = rhs.rotationX;
            node["rotationZ"] = rhs.rotationZ;
            return node;
        }

        static bool decode(const Node& node, CameraPersistence& rhs)
        {
            if (!node["position"] || !node["rotationX"] || !node["rotationZ"])
            {
                return false;
            }
            rhs.position = node["position"].as<glm::vec3>();
            rhs.rotationX = node["rotationX"].as<float>();
            rhs.rotationZ = node["rotationZ"].as<float>();
            return true;
        }
    };

    template <>
    struct convert<std::shared_ptr<CameraPersistence>>
    {
        static Node encode(const std::shared_ptr<CameraPersistence>& rhs)
        {
            return convert<CameraPersistence>::encode(*rhs);
        }

        static bool decode(const Node& node, std::shared_ptr<CameraPersistence>& rhs)
        {
            CameraPersistence value;
            bool decoded = convert<CameraPersistence>::decode(node, value);
            if (decoded)
            {
                rhs = std::make_shared<CameraPersistence>(value);
            }
            return decoded;
        }
    };
}
