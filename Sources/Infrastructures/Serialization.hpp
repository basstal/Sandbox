#pragma once
#include <boost/hana/at_key.hpp>
#include <boost/hana/define_struct.hpp>
#include <boost/hana/for_each.hpp>
#include <boost/hana/keys.hpp>
#include "yaml-cpp/node/node.h"

class Serialization
{
public:
	template <class T>
	static YAML::Node SerializeToYaml(const T& object);
	template <class T>
	static T Deserialize(const YAML::Node& node);
};


// 通用序列化函数
template <typename T>
YAML::Node Serialization::SerializeToYaml(const T& object)
{
	YAML::Node node;
	boost::hana::for_each(boost::hana::keys(object), [&](auto key)
	{
		auto value = boost::hana::at_key(object, key);
		std::string keyName = boost::hana::to<char const*>(key);
		if constexpr (std::is_enum_v<decltype(value)>)
		{
			node[keyName] = static_cast<int>(value);
		}
		else
		{
			node[keyName] = value;
		}
	});
	return node;
}

template <typename T>
T Serialization::Deserialize(const YAML::Node& node)
{
	T object; // 假设T类型有默认构造函数
	boost::hana::for_each(boost::hana::keys(object), [&](auto key)
	{
		auto& member = boost::hana::at_key(object, key); // 获取引用
		std::string keyName = boost::hana::to<char const*>(key);
		if (node[keyName])
		{
			using MemberType = std::decay_t<decltype(member)>; // 获取成员的类型，移除引用和const
			if constexpr (std::is_enum_v<MemberType>)
			{
				member = static_cast<MemberType>(node[keyName].as<int>()); // 需要自定义枚举和int之间的转换
			}
			else
			{
				member = node[keyName].as<MemberType>(); // 使用YAML-CPP的as函数进行类型转换
			}
		}
	});
	return object;
}
