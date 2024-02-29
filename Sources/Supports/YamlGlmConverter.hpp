#pragma once
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include "yaml-cpp/yaml.h"

namespace YAML
{
    template <>
    struct convert<glm::vec3>
    {
        static Node encode(const glm::vec3& rhs)
        {
            Node node;
            node["x"] = rhs.x;
            node["y"] = rhs.y;
            node["z"] = rhs.z;
            return node;
        }

        static bool decode(const Node& node, glm::vec3& rhs)
        {
            if (!node["x"] || !node["y"] || !node["z"])
            {
                return false;
            }
            rhs.x = node["x"].as<float>();
            rhs.y = node["y"].as<float>();
            rhs.z = node["z"].as<float>();
            return true;
        }
    };

    template <>
    struct convert<glm::vec4>
    {
        static Node encode(const glm::vec4& rhs)
        {
            Node node;
            node["x"] = rhs.x;
            node["y"] = rhs.y;
            node["z"] = rhs.z;
            node["w"] = rhs.w;
            return node;
        }

        static bool decode(const Node& node, glm::vec4& rhs)
        {
            if (!node["x"] || !node["y"] || !node["z"] || !node["w"])
            {
                return false;
            }
            rhs.x = node["x"].as<float>();
            rhs.y = node["y"].as<float>();
            rhs.z = node["z"].as<float>();
            rhs.w = node["w"].as<float>();
            return true;
        }
    };
}
