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

    //Transform functions
    template <bool CORDS_PARAMS_FLAG = true>
    constexpr FastMat Transform(const auto &Func) const
    {
        FastMat res;
        for (size_t i = 0; i < H; ++i)
        {
            for (size_t j = 0; j < W; ++j)
            {
                if constexpr (CORDS_PARAMS_FLAG)
                {
                    res.members[i][j] = Func(this->At(j, i), j, i);
                }
                else
                {
                    res.members[i][j] = Func(this->At(j, i));
                }
            }
        }
        return res;
    }
    template <bool CORDS_PARAMS_FLAG = false>
    constexpr FastMat Transform(const auto &Func, const FastMat &mat) const
    {
        FastMat res;
        for (size_t i = 0; i < H; ++i)
        {
            for (size_t j = 0; j < W; ++j)
            {
                if constexpr (CORDS_PARAMS_FLAG)
                {
                    res.members[i][j] = Func(this->At(j, i), mat.At(j, i), j, i);
                }
                else
                {
                    res.members[i][j] = Func(this->At(j, i), mat.At(j, i));
                }
            }
        }
        return res;
    }

    //ApplyFunction function
    template <bool CORDS_PARAMS_FLAG = false>
    constexpr void ApplyFunction(const auto &Func)
    {
        for (size_t i = 0; i < H; ++i)
        {
            for (size_t j = 0; j < W; ++j)
            {
                if constexpr (CORDS_PARAMS_FLAG)
                {
                    Func(this->At(j, i), j, i);
                }
                else
                {
                    Func(this->At(j, i));
                }
            }
        }
    }
    template <bool CORDS_PARAMS_FLAG = false>
    constexpr void ApplyFunction(const auto &Func, const FastMat &mat)
    {
        for (size_t i = 0; i < H; ++i)
        {
            for (size_t j = 0; j < W; ++j)
            {
                if constexpr (CORDS_PARAMS_FLAG)
                {
                    Func(this->At(j, i), mat.At(j, i), j, i);
                }
                else
                {
                    Func(this->At(j, i), mat.At(j, i));
                }
            }
        }
    }
};
