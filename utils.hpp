#pragma once
#include <type_traits>

//consteval for non-C++20
template <auto ret>
constexpr auto ConfirmConstexpr() { return ret; }

//CopyFast metaprogramming type
template <typename T>
struct copy_fast : std::conditional<std::is_trivially_copyable_v<T>, T, const T &> {};
template <typename T>
using copy_fast_t = typename copy_fast<T>::type;

#define INLINE __attribute__((always_inline)) inline

#define INDEX(v, i) v &(1 << i);
