#pragma once
#include <glm/vec3.hpp>
#include<boost/hana.hpp>

#include "Infrastructures/Math/Quaternion.hpp"
#include "Supports/YamlGlmConverter.hpp"

struct TransformPersistence
{
	glm::vec3 position = glm::vec3(0.0f);
	Quaternion rotation = Quaternion::identity;
	glm::vec3 scale = glm::vec3(1.0f);
};

BOOST_HANA_ADAPT_STRUCT(TransformPersistence, position, rotation, scale);


namespace YAML
{
	template <>
	struct convert<TransformPersistence>
	{
		static Node encode(const TransformPersistence& rhs)
		{
			Node node;
			node["position"] = rhs.position;
			node["rotation"] = rhs.rotation;
			node["scale"] = rhs.scale;
			return node;
		}

		static bool decode(const Node& node, TransformPersistence& rhs)
		{
			if (node["position"])
			{
				rhs.position = node["position"].as<glm::vec3>();
			}
			if (node["rotation"])
			{
				rhs.rotation = node["rotation"].as<Quaternion>();
			}
			if (node["scale"])
			{
				rhs.scale = node["scale"].as<glm::vec3>();
			}
			return true;
		}
	};
}
