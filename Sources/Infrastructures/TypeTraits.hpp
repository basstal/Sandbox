#pragma once

#include <type_traits>
#include <memory>

template <typename T>
struct extract_type; // 未定义通用版本


template <typename T>
struct extract_type<std::shared_ptr<T>>
{
	using type = T;
};


// 基本模板，假定不是 std::shared_ptr
template <typename T>
struct is_shared_ptr : std::false_type
{
};

// 模板偏特化，对 std::shared_ptr 特殊处理
template <typename T>
struct is_shared_ptr<std::shared_ptr<T>> : std::true_type
{
};
