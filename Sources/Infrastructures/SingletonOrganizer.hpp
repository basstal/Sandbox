#pragma once
#include <any>
#include <map>
#include <memory>
#include <stdexcept>
#include <typeindex>
#include <unordered_map>

class SingletonOrganizer
{
private:
	static std::map<int32_t, std::any> s_singletons;
	static std::unordered_map<std::type_index, int32_t> s_typeIndexToTypeId;
	static int32_t s_nextTypeId;

public:
	template <typename T>
	static void Register();
	template <typename T>
	static std::shared_ptr<T> &Get();
	template <typename T>
	static int32_t GetTypeId();
};

template <typename T>
void SingletonOrganizer::Register()
{
	int32_t typeId = GetTypeId<T>();
	std::shared_ptr<T> ptr = std::make_shared<T>();
	s_singletons.emplace(typeId, ptr);
}

template <typename T>
std::shared_ptr<T> &SingletonOrganizer::Get()
{
	try
	{
		return std::any_cast<std::shared_ptr<T>&>(s_singletons[GetTypeId<T>()]);
	}
	catch (const std::bad_any_cast&)
	{
		throw std::runtime_error("Singleton not found");
	}
}
template <typename T>
int32_t SingletonOrganizer::GetTypeId()
{
	std::type_index typeIndex(typeid(T));
	if (!s_typeIndexToTypeId.contains(typeIndex))
	{
		s_typeIndexToTypeId[typeIndex] = s_nextTypeId++;
	}
	return s_typeIndexToTypeId[typeIndex];
}
