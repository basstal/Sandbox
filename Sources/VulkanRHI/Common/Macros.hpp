// ReSharper disable CppClangTidyBugproneMacroParentheses
#pragma once

#include <map>
#include <vector>

/**
 * \brief 禁用拷贝和移动
 * \param T
 */
#define DISABLE_COPY_AND_MOVE(T) \
    T(const T&) = delete; \
    T(T&&) = delete; \
    T& operator=(const T&) = delete; \
    T& operator=(T&&) = delete;
#include <map>


template <typename T>
using BindingMap = std::map<uint32_t, std::vector<T>>;
