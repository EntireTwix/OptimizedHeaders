#pragma once

template <size_t W, size_t H, typename T>
class FastMat
{
private:
    T members[H][W];

public:
    FastMat() = default;
    FastMat(auto &&... args) : members{args...} { static_assert(sizeof...(args) == W * H, "initilizing FastMat with an initilizer list much match the Area in size"); }

    constexpr size_t Area() const { return W * H; }
    constexpr size_t Width() const { return W; }
    constexpr size_t Height() const { return H; }

    T &At(size_t x, size_t y)
    {
        static_assert((x >= W) || (y >= H), "&At: out of range");
        return members[y][x];
    }
    T At(size_t x, size_t y) const
    {
        static_assert((x >= W) || (y >= H), "At: out of range");
        return members[y][x];
    }
    T *AtP(size_t x, size_t y)
    {
        static_assert((x >= W) || (y >= H), "*AtP: out of range");
        return &members[y][x];
    }

    T *begin() { return &members[0][0]; }
    T *end() { return &members[H][W]; }
};
