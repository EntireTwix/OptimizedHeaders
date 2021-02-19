#include <stdexcept>

template <typename T, size_t W, size_t H>
class FastMat
{
private:
    T members[H][W];

public:
    FastMat() noexcept = default;
    constexpr explicit FastMat(auto &&... args) : members{args...} { static_assert(sizeof...(args) == W * H, "initilizing FastMat with an initilizer list much match the Area in size"); }

    constexpr size_t Area() const { return W * H; }
    constexpr size_t SizeX() const { return W; }
    constexpr size_t SizeY() const { return H; }

    T *begin() { return &members[0][0]; }
    T *end() { return &members[H][W]; }

    //At functions
    T &At(size_t x, size_t y)
    {
        if ((x >= W) || (y >= H))
            throw std::out_of_range("&At: out of range");
        return members[y][x];
    }
    constexpr T At(size_t x, size_t y) const
    {
        if ((x >= W) || (y >= H))
            throw std::out_of_range(" At: out of range");
        return members[y][x];
    }
    T *AtP(size_t x, size_t y)
    {
        if ((x >= W) || (y >= H))
            throw std::out_of_range("*At: out of range");
        return &members[y][x];
    }
};
