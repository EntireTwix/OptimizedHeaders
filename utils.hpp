#pragma once
//note this header requires C++20

template <auto ret>
constexpr auto ConfirmConstexpr() { return ret; }
