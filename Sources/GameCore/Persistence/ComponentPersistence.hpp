#pragma once
#include <cstdint>
#include <boost/hana.hpp>
#include <yaml-cpp/yaml.h>

struct ComponentPersistence
{
	int32_t id;
};

BOOST_HANA_ADAPT_STRUCT(ComponentPersistence, id);

namespace YAML
{
	template <>
	struct convert<ComponentPersistence>
	{
		static Node encode(const ComponentPersistence& rhs)
		{
			Node node;
			node["id"] = rhs.id;
			return node;
		}

		static bool decode(const Node& node, ComponentPersistence& rhs)
		{
			if (!node["id"])
			{
				return false;
			}
			rhs.id = node["id"].as<int32_t>();
			return true;
		}
	};
}
