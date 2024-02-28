#include "SingletonOrganizer.hpp"

std::map<int32_t, std::any> SingletonOrganizer::m_singletons = {};
std::unordered_map<std::type_index, int32_t> SingletonOrganizer::m_typeIndexToTypeId = {};
int32_t SingletonOrganizer::m_nextTypeId = 0;
