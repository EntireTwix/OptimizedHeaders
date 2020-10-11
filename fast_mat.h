#pragma once

template <size_t W, size_t H, typename T>
class FastMat
{
private:
    T members[H][W];

public:
    FastMat() = default;

    template <typename... Params>
    FastMat(Params &&... args) : members{args...} {}

    constexpr size_t Area() const { return W * H; }
    constexpr size_t Width() const { return W; }
    constexpr size_t Height() const { return H; }

    T &At(size_t x, size_t y) { return members[y][x]; }
    T At(size_t x, size_t y) const { return members[y][x]; }

    T *begin() { return &members[0][0]; }
    T *end() { return &members[H][W]; }
};
