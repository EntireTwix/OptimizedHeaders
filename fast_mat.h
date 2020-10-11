#pragma once

template <size_t W, size_t H, typename T>
class FastMat
{
private:
    T members[W * H];

public:
    FastMat() = default;

    template <typename... Params>
    FastMat(Params &&... args) : members{args...} {}

    constexpr size_t Area() const { return W * H; }
    constexpr size_t Width() const { return W; }
    constexpr size_t Height() const { return H; }
};