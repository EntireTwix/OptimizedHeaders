#include <stdexcept>

template <size_t W, size_t H, typename T>
class FastMat
{
private:
    T members[H][W];

public:
    FastMat() noexcept = default;
    constexpr explicit FastMat(auto &&... args) : members{args...} { static_assert(sizeof...(args) == W * H, "initilizing FastMat with an initilizer list much match the Area in size"); }

    constexpr size_t Area() const { return W * H; }
    constexpr size_t Width() const { return W; }
    constexpr size_t Height() const { return H; }

    T &At(size_t x, size_t y);
    constexpr T At(size_t x, size_t y) const;
    T *AtP(size_t x, size_t y);

    T *begin() { return &members[0][0]; }
    T *end() { return &members[H][W]; }
};

template <size_t W, size_t H, typename T>
inline T &FastMat<W, H, T>::At(size_t x, size_t y)
{
    if ((x >= W) || (y >= H))
        throw std::out_of_range("At: out of range");
    return members[y][x];
}

template <size_t W, size_t H, typename T>
constexpr inline T FastMat<W, H, T>::At(size_t x, size_t y) const
{
    if ((x >= W) || (y >= H))
        throw std::out_of_range(" At: out of range");
    return members[y][x];
}

template <size_t W, size_t H, typename T>
inline T *FastMat<W, H, T>::AtP(size_t x, size_t y)
{
    if ((x >= W) || (y >= H))
        throw std::out_of_range("*At: out of range");
    return members[y][x];
}
