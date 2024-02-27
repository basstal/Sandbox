#include "SingletonOrganizer.hpp"

std::map<int32_t, std::any> SingletonOrganizer::s_singletons = {};
std::unordered_map<std::type_index, int32_t> SingletonOrganizer::s_typeIndexToTypeId = {};
int32_t SingletonOrganizer::s_nextTypeId = 0;
