#pragma once
#include <string>
#include<boost/hana.hpp>

struct ModelPersistence
{
	std::string modelPath;
};

BOOST_HANA_ADAPT_STRUCT(ModelPersistence, modelPath);
