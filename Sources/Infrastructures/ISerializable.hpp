#pragma once
#include "yaml-cpp/node/node.h"

class ISerializable
{
public:
	virtual ~ISerializable() = default;
	virtual std::string GetSerializedPath() = 0;
	virtual void Save() = 0;
	virtual void Load() = 0;
};
