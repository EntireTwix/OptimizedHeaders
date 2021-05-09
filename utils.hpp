#pragma once
#include <type_traits>

//consteval for non-C++20
template <auto ret>
constexpr auto ConfirmConstexpr() { return ret; }
//CopyFast metaprogramming type
template <typename T>
struct copy_fast : std::conditional<std::is_trivially_copyable_v<T>, T, const T &>{};
template <typename T>
struct copy_fast_cv : copy_fast<typename std::remove_cvref_t<T>>{};
template <typename T>
using copy_fast_cv_t = typename copy_fast_cv<T>::type;

#define INLINE __attribute__((always_inline)) inline

#define GET(v, i) v &(1 << i)
#define SET(v, i) v ^= (1 << i)
#define ON(v, i) v |= (1 << i)
#define OFF(v, i) v &= ~(1 << i)

#include <ctime>
#include <ratio>
#include <chrono>
#define time(f)                                                            \
    {using namespace std::chrono;                                           \
    high_resolution_clock::time_point t1 = high_resolution_clock::now();   \
    f;                                                                     \
    high_resolution_clock::time_point t2 = high_resolution_clock::now();   \
    duration<double> time_span = duration_cast<duration<double>>(t2 - t1); \
    std::cout << "It took me " << time_span.count() << " seconds.\n";}
