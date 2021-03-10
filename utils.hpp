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

//CombineBits
template <typename T>
constexpr unsigned char* CombineBits(T val) { return val; }
template <typename T, typename... Bits>
constexpr unsigned char* CombineBits(T head, Bits... vars) { return (head << (sizeof(T)*8)) + CombineBits(vars...); }
