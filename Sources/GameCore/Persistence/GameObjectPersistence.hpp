#pragma once
#include <string>
#include <vector>
#include<boost/hana.hpp>

#include "ComponentPersistence.hpp"
#include "TransformPersistence.hpp"

struct ComponentPersistence;

struct GameObjectPersistence
{
	TransformPersistence transform;
	std::vector<ComponentPersistence> components;
};

BOOST_HANA_ADAPT_STRUCT(GameObjectPersistence, transform);

namespace YAML
{
	template <>
	struct convert<GameObjectPersistence>
	{
		static Node encode(const GameObjectPersistence& rhs)
		{
			Node node;
			node["transform"] = rhs.transform;
			node["components"] = rhs.components;
			return node;
		}

		static bool decode(const Node& node, GameObjectPersistence& rhs)
		{
			if (node["transform"])
			{
				rhs.transform = node["transform"].as<TransformPersistence>();
			}
			if (node["components"])
			{
				rhs.components = node["components"].as<std::vector<ComponentPersistence>>();
			}
			return true;
		}
	};
}
