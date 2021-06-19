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

#define time_func(f, x)                                                                       \
    {                                                                                         \
        using namespace std::chrono;                                                          \
        uint32_t timer = 0;                                                                   \
        for (int i = 0; i < x; ++i)                                                           \
        {                                                                                     \
            auto t1 = high_resolution_clock::now().time_since_epoch();                        \
            f;                                                                                \
            auto t2 = high_resolution_clock::now().time_since_epoch();                        \
            timer += std::chrono::duration_cast<std::chrono::nanoseconds>((t2 - t1)).count(); \
        }                                                                                     \
        std::cout << timer / x << '\n';                                                       \
    }

#define Op(v, name, x)     \
    {                      \
        std::cout << name; \
        time_func(v, x);   \
    }

