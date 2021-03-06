#include <stdexcept>

template <size_t W, size_t H, typename T = float>
class FastMat
{
private:
    T members[H][W];

public:
    constexpr FastMat() noexcept = default;
    constexpr explicit FastMat(T &&head, auto &&...args) : members{head, args...} { static_assert(sizeof...(args) + 1 == W * H, "initilizing FastMat with an initilizer list much match the Area in size"); }

    constexpr size_t Area() const noexcept { return W * H; }
    constexpr size_t SizeX() const noexcept { return W; }
    constexpr size_t SizeY() const noexcept { return H; }

    constexpr T *begin() noexcept { return &members[0][0]; }
    constexpr T *end() noexcept { return &members[H - 1][W]; }
    constexpr const T *begin() const noexcept { return &members[0][0]; }
    constexpr const T *end() const noexcept { return &members[H - 1][W]; }

    //At functions
    constexpr T &At(size_t x, size_t y)
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
    constexpr T *AtP(size_t x, size_t y)
    {
        if ((x >= W) || (y >= H))
            throw std::out_of_range("*At: out of range");
        return &members[y][x];
    }

    friend std::ostream &operator<<(std::ostream &os, const FastMat &mat)
    {
        for (size_t i = 0; i < mat.SizeY(); ++i)
        {
            for (size_t j = 0; j < mat.SizeX(); ++j)
            {
                os << mat.At(j, i) << ' ';
            }
            os << '\n';
        }
        return os;
    }
};
