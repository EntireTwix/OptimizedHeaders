#pragma once
#include <type_traits>

#ifdef _MSC_VER
#define ALWAYS_INLINE __forceinline
#else
#define ALWAYS_INLINE inline __attribute((__always_inline__))
#endif

//consteval for non-C++20
template <auto ret>
constexpr auto ConfirmConstexpr() { return ret; }

//CopyFast metaprogramming type
template <typename T>
struct copy_fast : std::conditional<std::is_trivially_copyable_v<T>, T, const T &>{};
template <typename T>
using copy_fast_t = typename copy_fast<T>::type;

#define HAS_(member, member_type)                                         \
    template <typename T, typename = member_type>                         \
    struct has_##member : std::false_type                                 \
    {                                                                     \
    };                                                                    \
    template <typename T>                                                 \
    struct has_##member<T, decltype((void)T::member, 0)> : std::true_type \
    {                                                                     \
    };    

#define HAS_FUNC_(func_name)                                                                                                                  \
    template <typename, typename T>                                                                                                           \
    struct has_##func_name                                                                                                                    \
    {                                                                                                                                         \
        static_assert(std::integral_constant<T, false>::value, "Second template parameter needs to be of function type.");                    \
    };                                                                                                                                        \
    template <typename C, typename Ret, typename... Args>                                                                                     \
    struct has_##func_name<C, Ret(Args...)>                                                                                                   \
    {                                                                                                                                         \
    private:                                                                                                                                  \
        template <typename T>                                                                                                                 \
        static constexpr auto check(T *) -> typename std::is_same<decltype(std::declval<T>().func_name(std::declval<Args>()...)), Ret>::type; \
        template <typename>                                                                                                                   \
        static constexpr std::false_type check(...);                                                                                          \
        typedef decltype(check<C>(0)) type;                                                                                                   \
    public:                                                                                                                                   \
        static constexpr bool value = type::value;                                                                                            \
    } 

#define HAS_STATIC_FUNC(static_method_name)                                                                                             \
    template <typename, typename T>                                                                                                     \
    struct has_##static_method_name                                                                                                     \
    {                                                                                                                                   \
        static_assert(std::integral_constant<T, false>::value, "Second template parameter needs to be of function type.");              \
    };                                                                                                                                  \
    template <typename C, typename Ret, typename... Args>                                                                               \
    struct has_##static_method_name<C, Ret(Args...)>                                                                                    \
    {                                                                                                                                   \
        template <typename T>                                                                                                           \
        static constexpr auto check(T *) -> typename std::is_same<decltype(T::static_method_name(std::declval<Args>()...)), Ret>::type; \
        template <typename>                                                                                                             \
        static constexpr std::false_type check(...);                                                                                    \
        static constexpr bool value = decltype(check<C>(nullptr))::value;                                                               \
    };

#define GET(v, i) v &(1 << i)
#define SET(v, i) v ^= (1 << i)
#define ON(v, i) v |= (1 << i)
#define OFF(v, i) v &= ~(1 << i)

#include <ctime>
#include <ratio>
#include <chrono>

#define time_func_a(f, a, x)                                                                  \
    {                                                                                         \
        using namespace std::chrono;                                                          \
        uint32_t timer = 0;                                                                   \
        for (int i = 0; i < x; ++i)                                                           \
        {                                                                                     \
            auto t1 = high_resolution_clock::now().time_since_epoch();                        \
            f;                                                                                \
            auto t2 = high_resolution_clock::now().time_since_epoch();                        \
            a;                                                                                \
            timer += std::chrono::duration_cast<std::chrono::nanoseconds>((t2 - t1)).count(); \
        }                                                                                     \
        std::cout << timer / x << '\n';                                                       \
    }

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

#define Op_a(v, name, x, a)   \
    {                         \
        std::cout << name;    \
        time_func_a(v, a, x); \
    }

#define Op(v, name, x)     \
    {                      \
        std::cout << name; \
        time_func(v, x);   \
    }

template <typename T>
constexpr T pow2(copy_fast_t<T> val) { return val * val; }
