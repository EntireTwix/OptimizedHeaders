#include <stdexcept>
#include <functional>
#include <memory>

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
    FastMat Transform(const std::function<T(const T &)> &Func) const
    {
        FastMat res;
        for (size_t i = 0; i < H; ++i)
        {
            for (size_t j = 0; j < W; ++j)
            {
                res.members[i][j] = Func(this->At(j, i));
            }
        }
        return res;
    }
    template <typename ConstexprFunc>
    constexpr FastMat Transform(const ConstexprFunc &Func, bool CORD_PARAMS = false) const
    {
        FastMat res;
        for (size_t i = 0; i < H; ++i)
        {
            for (size_t j = 0; j < W; ++j)
            {
                if (CORD_PARAMS)
                    res.members[i][j] = Func(this->At(j, i), j, i);
                else
                    res.members[i][j] = Func(this->At(j, i));
            }
        }
        return res;
    }
    FastMat Transform(std::function<T(const T &, const T &)> Func, const FastMat &mat) const
    {
        FastMat res;
        for (size_t i = 0; i < H; ++i)
        {
            for (size_t j = 0; j < W; ++j)
            {
                res.members[i][j] = Func(this->At(j, i), mat.At(j, i));
            }
        }
        return res;
    }
    template <typename ConstexprFunc>
    constexpr FastMat Transform(const ConstexprFunc &Func, const FastMat &mat, bool CORD_PARAMS = false) const
    {
        FastMat res;
        for (size_t i = 0; i < H; ++i)
        {
            for (size_t j = 0; j < W; ++j)
            {
                if (CORD_PARAMS)
                    res.members[i][j] = Func(this->At(j, i), j, i);
                else
                    res.members[i][j] = Func(this->At(j, i));
            }
        }
        return res;
    }
    FastMat Transform(const std::function<T(const T &, size_t, size_t)> &Func) const
    {
        FastMat res;
        for (size_t i = 0; i < H; ++i)
        {
            for (size_t j = 0; j < W; ++j)
            {
                res.members[i][j] = Func(this->At(j, i), j, i);
            }
        }
        return res;
    }
    FastMat Transform(std::function<T(const T &, const T &, size_t, size_t)> Func, const FastMat &mat) const
    {
        FastMat res;
        for (size_t i = 0; i < H; ++i)
        {
            for (size_t j = 0; j < W; ++j)
            {
                res.members[i][j] = Func(this->At(j, i), mat.At(j, i), j, i);
            }
        }
        return res;
    }

    //ApplyFunction function
    void ApplyFunction(const std::function<void(T &)> &Func)
    {
        for (size_t i = 0; i < H; ++i)
        {
            for (size_t j = 0; j < W; ++j)
            {
                this->At(i, j) = Func(this->At(j, i));
            }
        }
    }
    void ApplyFunction(const std::function<void(T &, size_t, size_t)> &Func)
    {
        for (size_t i = 0; i < H; ++i)
        {
            for (size_t j = 0; j < W; ++j)
            {
                this->At(i, j) = Func(this->At(j, i), j, i);
            }
        }
    }
};
