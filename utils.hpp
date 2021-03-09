#pragma once
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
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
inline unsigned char* CombineBits(T val) { return val; }
template <typename T, typename... Bits>
inline unsigned char* CombineBits(T head, Bits... vars) { return (head << (sizeof(T)*8)) + CombineBits(vars...); }

//File I/O
inline std::vector<std::string> ReadFileSimple(const std::string &file, char delim)
{
    std::ifstream temp(file);
    std::vector<std::string> res;
    std::string line;
    if (temp.is_open())
    {
        while (std::getline(temp, line, delim))
        {
            res.push_back(line);
        }
        temp.close();
    }
    return res;
}
template <typename T>
inline std::vector<std::string> ReadFileSimple(const std::string &file, char delim, const T &func)
{
    std::ifstream temp(file);
    std::vector<std::string> res;
    std::string line;
    if (temp.is_open())
    {
        while (std::getline(temp, line, delim))
        {
            func(line);
            res.push_back(line);
        }
        temp.close();
    }
    return res;
}

//string Split
inline std::vector<std::string> Split(const std::string &inp, char delim)
{
    std::stringstream ss(inp);
    std::vector<std::string> res;
    std::string line;
    while (std::getline(ss, line, delim))
    {
        res.push_back(line);
    }
    return res;
}
template <typename T>
inline std::vector<std::string> Split(const std::string &inp, char delim, const T &func)
{
    std::stringstream ss(inp);
    std::vector<std::string> res;
    std::string line;
    while (std::getline(ss, line, delim))
    {
        func(line);
        res.push_back(line);
    }
    return res;
}
