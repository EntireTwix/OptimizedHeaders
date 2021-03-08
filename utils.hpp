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
unsigned char* CombineBits(T val) { return val; }
template <typename T, typename... Bits>
unsigned char* CombineBits(T head, Bits... vars) { return (head << sizeof(T)) + CombineBits(vars...); }

/* nlohmann/json I/O 

template <typename T>
bool Load(const std::string &location, T &dest)
{
    std::ifstream input(location);
    json temp;
    std::string temp_str, final_str;
    if (input.is_open())
    {
        while (input >> temp_str)
        {
            final_str += temp_str;
        }
        if (final_str == "")
        {
            return false;
        }
        temp = json::parse(final_str);
        input.close();
    }
    else
    {
        return false;
    }

    dest = (T)temp;
    return true;
}

//THIS FUNCTION ONLY EXISTS BECAUSE MSVC WILL THROW A FIT ABOUT THE ABOVE LOAD FUNCTION (DO NOT REMOVE)
template <typename T>
bool Load(const std::string& location, std::vector<T>& dest)
{
    std::ifstream input(location);
    json temp;
    std::string temp_str, final_str
    if (input.is_open())
    {
        while (input >> temp_str)
        {
            final_str += temp_str;
        }
        if (temp_str == "")
        {
            return false;
        }
        temp = json::parse(final_str);
        input.close();
    }
    else
    {
        return false;
    }

    dest.resize(temp.size());
    for (size_t i = 0; i < temp.size(); ++i)
    {
        dest[i] = (T)temp[i];
    }
    return true;
}

template <typename T>
bool Save(const std::string &location, const T &dest)
{
    std::ofstream output(location);
    if (output.is_open())
    {
        output << (json(dest).dump());
    }
    else
    {
        return false;
    }
    output.close();
    return true;
}

*/
