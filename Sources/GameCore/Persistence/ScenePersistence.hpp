#pragma once
#include <vector>
#include<boost/hana.hpp>



struct GameObjectPersistence;

struct ScenePersistence
{
	std::vector<GameObjectPersistence> gameObjects;
};


BOOST_HANA_ADAPT_STRUCT(ScenePersistence, gameObjects);
