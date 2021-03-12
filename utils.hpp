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
template <typename T, typename T2>
constexpr auto CombineBits(T a, T2 b) { return a ^ b << ((sizeof(T) * 8) - 1); }
template <typename T, typename... Bits>
constexpr auto CombineBits(T head, Bits... bits) { return CombineBits(bits...) << ((sizeof(T) * 8) - 1) ^ head; }
