#pragma once

#include <memory>
#include <type_traits>

template <typename T>
struct ExtractType;  // 未定义通用版本


template <typename T>
struct ExtractType<std::shared_ptr<T>>
{
    using type = T;
};


// 基本模板，假定不是 std::shared_ptr
template <typename T>
struct IsSharedPtr : std::false_type
{
};

// 模板偏特化，对 std::shared_ptr 特殊处理
template <typename T>
struct IsSharedPtr<std::shared_ptr<T>> : std::true_type
{
};
