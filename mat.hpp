#pragma once
#include <stdexcept>
#include <string>
#include <ostream>
#include <concepts>
#include <type_traits>

template <typename T>
struct copy_fast : std::conditional<std::is_trivially_copyable_v<T>, T, const T &>
{
};

template <typename T>
using copy_fast_t = typename copy_fast<T>::type;

template <typename T>
concept SizeType = std::is_unsigned_v<T> &&std::is_integral_v<T>;

template <typename Type = float, SizeType SizeT = size_t>
class Mat
{
private:
    Type *members = nullptr;
    SizeT sizeY, sizeX;

public:
    using type = Type;
    using storage_type = SizeT;

    Mat() = default;
    explicit Mat(SizeT, SizeT);

    template <typename... Params>
    explicit Mat(SizeT w, SizeT h, Params &&...membs) : sizeX(w), sizeY(h)
    {
        if (sizeof...(membs) > (w * h))
        {
            throw std::invalid_argument("dimensions of matrix must match number of values");
        }
        members = new Type[w * h]{membs...};
    }

    Mat(const Mat &);
    Mat(Mat &&) noexcept;
    Mat operator=(const Mat &);
    Mat operator=(Mat &&) noexcept;

    Type &At(SizeT, SizeT);
    Type *AtP(SizeT, SizeT);
    Type At(SizeT, SizeT) const;

    Type &FastAt(size_t);
    Type FastAt(size_t) const;
    Type *FastAtP(size_t);

    SizeT SizeX() const noexcept;
    SizeT SizeY() const noexcept;

    Mat Dot(const Mat &) const;

    Mat operator+(const Mat &) const;
    Mat operator+(copy_fast_t<Type>) const noexcept;
    Mat operator+=(const Mat &);
    Mat operator+=(copy_fast_t<Type>) noexcept;

    Mat operator-(const Mat &) const;
    Mat operator-(copy_fast_t<Type>) const noexcept;
    Mat operator-=(const Mat &);
    Mat operator-=(copy_fast_t<Type>) noexcept;

    Mat operator*(const Mat &) const;
    Mat operator*(copy_fast_t<Type>) const noexcept;
    Mat operator*=(const Mat &);
    Mat operator*=(copy_fast_t<Type>) noexcept;

    Mat operator/(const Mat &) const;
    Mat operator/(copy_fast_t<Type>) const noexcept;
    Mat operator/=(const Mat &);
    Mat operator/=(copy_fast_t<Type>) noexcept;

    Type *begin() noexcept { return &members[0]; }
    Type *end() noexcept { return &members[sizeX * sizeY]; }
    const Type *begin() const noexcept { return &members[0]; }
    const Type *end() const noexcept { return &members[sizeX * sizeY]; }
    Type *&data() noexcept { return members; } //dangerous

    friend std::ostream &operator<<(std::ostream &os, const Mat &mat) noexcept
    {
        for (SizeT i = 0; i < mat.SizeY(); ++i)
        {
            for (SizeT j = 0; j < mat.SizeX(); ++j)
            {
                os << mat.At(j, i) << ' ';
            }
            os << '\n';
        }
        return os;
    }

    std::string Save() const noexcept
    {
        using namespace std;
        std::string res('(' + std::to_string(sizeX) + ',' + std::to_string(sizeY) + ',');
        for (copy_fast_t<Type> i : *this)
        {
            res += to_string(i) + ',';
        }
        res.back() = ')';
        return res;
    }

    ~Mat() noexcept;
};

template <typename Type, SizeType SizeT>
inline Mat<Type, SizeT>::Mat(SizeT w, SizeT h) : sizeX(w), sizeY(h)
{
    members = new Type[w * h]{};
}

template <typename Type, SizeType SizeT>
inline Mat<Type, SizeT>::Mat(const Mat<Type, SizeT> &mat)
{
    sizeX = mat.sizeX;
    sizeY = mat.sizeY;
    if (sizeX * sizeY)
    {
        members = new Type[sizeX * sizeY]{Type()};
    }
    for (size_t i = 0; i < mat.sizeX * sizeY; ++i)
    {
        FastAt(i) = mat.FastAt(i);
    }
}

template <typename Type, SizeType SizeT>
inline Mat<Type, SizeT>::Mat(Mat<Type, SizeT> &&mat) noexcept
{
    sizeX = mat.sizeX;
    sizeY = mat.sizeY;
    members = mat.members;
    mat.members = nullptr;
    mat.sizeX = mat.sizeY = 0;
}

template <typename Type, SizeType SizeT>
inline Mat<Type, SizeT> Mat<Type, SizeT>::operator=(const Mat<Type, SizeT> &mat)
{
    sizeX = mat.sizeX;
    sizeY = mat.sizeY;
    if (members)
    {
        delete[] members;
        members = nullptr;
    }
    if (sizeX * sizeY)
    {
        members = new Type[sizeX * sizeY]{Type()};
    }
    for (size_t i = 0; i < sizeX * sizeY; ++i)
    {
        FastAt(i) = mat.FastAt(i);
    }
    return *this;
}

template <typename Type, SizeType SizeT>
inline Mat<Type, SizeT> Mat<Type, SizeT>::operator=(Mat<Type, SizeT> &&mat) noexcept
{
    sizeX = mat.sizeX;
    sizeY = mat.sizeY;
    if (members)
    {
        delete[] members;
    }
    members = mat.members;
    mat.members = nullptr;
    mat.sizeX = mat.sizeY = 0;
    return *this;
}

template <typename Type, SizeType SizeT>
inline Type &Mat<Type, SizeT>::At(SizeT x, SizeT y) //indexing matrix
{
    if ((x >= sizeX) || (y >= sizeY))
        throw std::out_of_range("At: out of range, " + std::to_string(x) + ' ' + std::to_string(y));
    return members[(y * sizeX) + x];
}

template <typename Type, SizeType SizeT>
inline Type *Mat<Type, SizeT>::AtP(SizeT x, SizeT y)
{
    if ((x >= sizeX) || (y >= sizeY))
        throw std::out_of_range("At: out of range, " + std::to_string(x) + ' ' + std::to_string(y));
    return &members[(y * sizeX) + x];
}

template <typename Type, SizeType SizeT>
inline Type Mat<Type, SizeT>::At(SizeT x, SizeT y) const //indexing matrix
{
    if ((x >= sizeX) || (y >= sizeY))
        throw std::out_of_range("At: out of range, " + std::to_string(x) + ' ' + std::to_string(y));
    return members[(y * sizeX) + x];
}

template <typename Type, SizeType SizeT>
inline Type &Mat<Type, SizeT>::FastAt(size_t index)
{
    if (index >= sizeX * sizeY)
        throw std::out_of_range("FastAt: index out of range");
    return members[index];
}

template <typename Type, SizeType SizeT>
inline Type Mat<Type, SizeT>::FastAt(size_t index) const
{
    if (index >= sizeX * sizeY)
        throw std::out_of_range("FastAt: index out of range");
    return members[index];
}

template <typename Type, SizeType SizeT>
inline Type *Mat<Type, SizeT>::FastAtP(size_t index)
{
    if (index >= sizeX * sizeY)
        throw std::out_of_range("FastAt: index out of range");
    return &members[index];
}

template <typename Type, SizeType SizeT>
inline SizeT Mat<Type, SizeT>::SizeX() const noexcept { return sizeX; }

template <typename Type, SizeType SizeT>
inline SizeT Mat<Type, SizeT>::SizeY() const noexcept { return sizeY; }

template <typename Type, SizeType SizeT>
inline Mat<Type, SizeT> Mat<Type, SizeT>::Dot(const Mat<Type, SizeT> &mat) const
{
    if (!((sizeX == mat.sizeY) || (sizeY == mat.sizeX)))
    {
        throw std::invalid_argument("Dot: dimensions invalid");
    }
    if (sizeX == mat.sizeY)
    {
        //std::cout << "Beep\n";
        Mat res(mat.sizeX, sizeY);        //product dimensions are the x of both mats
        for (SizeT i = 0; i < sizeY; ++i) //for each y on the A matrix
        {
            for (SizeT j = 0; j < mat.sizeX; ++j) //for each x on the B matrix
            {
                for (int k = 0; k < sizeX; ++k) //is equivelant to mat.sizeY()
                {
                    res.At(j, i) += At(k, i) * mat.At(j, k);
                }
            }
        }
        return res;
    }
    else
    {
        //std::cout << "Boop\n";
        Mat res(sizeX, mat.sizeY);
        for (SizeT i = 0; i < mat.sizeY; ++i)
        {
            for (SizeT j = 0; j < sizeX; ++j)
            {
                for (int k = 0; k < mat.sizeX; ++k)
                {
                    res.At(j, i) += At(j, k) * mat.At(k, i);
                }
            }
        }
        return res;
    }
}

template <typename Type, SizeType SizeT>
inline Mat<Type, SizeT> Mat<Type, SizeT>::operator+(const Mat<Type, SizeT> &mat) const
{
    if ((sizeX * sizeY) != (mat.sizeX * mat.sizeY))
        throw std::invalid_argument("operator+: parameter must match the dimensions of this");
    Mat res(sizeX, sizeY);
    for (size_t i = 0; i < sizeX * sizeY; ++i)
    {
        res.members[i] = this->members[i] + mat.members[i];
    }
    return res;
}

template <typename Type, SizeType SizeT>
inline Mat<Type, SizeT> Mat<Type, SizeT>::operator+(copy_fast_t<Type> value) const noexcept
{
    Mat res(sizeX, sizeY);
    for (size_t i = 0; i < sizeX * sizeY; ++i)
    {
        res.members[i] = this->members[i] + value;
    }
    return res;
}

template <typename Type, SizeType SizeT>
inline Mat<Type, SizeT> Mat<Type, SizeT>::operator+=(const Mat<Type, SizeT> &mat)
{
    if ((sizeX * sizeY) != (mat.sizeX * mat.sizeY))
        throw std::invalid_argument("operator+=: parameter must match the dimensions of this");
    for (size_t i = 0; i < sizeX * sizeY; ++i)
    {
        this->members[i] += mat.members[i];
    }
}

template <typename Type, SizeType SizeT>
inline Mat<Type, SizeT> Mat<Type, SizeT>::operator+=(copy_fast_t<Type> value) noexcept
{
    for (size_t i = 0; i < sizeX * sizeY; ++i)
    {
        this->members[i] += value;
    }
    return *this;
}

template <typename Type, SizeType SizeT>
inline Mat<Type, SizeT> Mat<Type, SizeT>::operator-(const Mat<Type, SizeT> &mat) const
{
    if ((sizeX * sizeY) != (mat.sizeX * mat.sizeY))
        throw std::invalid_argument("operator-: parameter must match the dimensions of this");
    Mat res(sizeX, sizeY);
    for (size_t i = 0; i < sizeX * sizeY; ++i)
    {
        res.members[i] = this->members[i] - mat.members[i];
    }
    return res;
}

template <typename Type, SizeType SizeT>
inline Mat<Type, SizeT> Mat<Type, SizeT>::operator-(copy_fast_t<Type> value) const noexcept
{
    Mat res(sizeX, sizeY);
    for (size_t i = 0; i < sizeX * sizeY; ++i)
    {
        res.members[i] = this->members[i] - value;
    }
    return res;
}

template <typename Type, SizeType SizeT>
inline Mat<Type, SizeT> Mat<Type, SizeT>::operator-=(const Mat<Type, SizeT> &mat)
{
    if ((sizeX * sizeY) != (mat.sizeX * mat.sizeY))
        throw std::invalid_argument("operator-=: parameter must match the dimensions of this");
    for (size_t i = 0; i < sizeX * sizeY; ++i)
    {
        this->members[i] -= mat.members[i];
    }
    return *this;
}

template <typename Type, SizeType SizeT>
inline Mat<Type, SizeT> Mat<Type, SizeT>::operator-=(copy_fast_t<Type> value) noexcept
{
    for (size_t i = 0; i < sizeX * sizeY; ++i)
    {
        this->members[i] -= value;
    }
    return *this;
}

template <typename Type, SizeType SizeT>
inline Mat<Type, SizeT> Mat<Type, SizeT>::operator*(const Mat<Type, SizeT> &mat) const
{
    if ((sizeX * sizeY) != (mat.sizeX * mat.sizeY))
        throw std::invalid_argument("operator*: parameter must match the dimensions of this");
    Mat res(sizeX, sizeY);
    for (size_t i = 0; i < sizeX * sizeY; ++i)
    {
        res.members[i] = this->members[i] * mat.members[i];
    }
    return res;
}

template <typename Type, SizeType SizeT>
inline Mat<Type, SizeT> Mat<Type, SizeT>::operator*(copy_fast_t<Type> value) const noexcept
{
    Mat res(sizeX, sizeY);
    for (size_t i = 0; i < sizeX * sizeY; ++i)
    {
        res.members[i] = this->members[i] * value;
    }
    return res;
}

template <typename Type, SizeType SizeT>
inline Mat<Type, SizeT> Mat<Type, SizeT>::operator*=(const Mat<Type, SizeT> &mat)
{
    if ((sizeX * sizeY) != (mat.sizeX * mat.sizeY))
        throw std::invalid_argument("operator*=: parameter must match the dimensions of this");
    for (size_t i = 0; i < sizeX * sizeY; ++i)
    {
        this->members[i] *= mat.members[i];
    }
    return *this;
}

template <typename Type, SizeType SizeT>
inline Mat<Type, SizeT> Mat<Type, SizeT>::operator*=(copy_fast_t<Type> value) noexcept
{
    for (size_t i = 0; i < sizeX * sizeY; ++i)
    {
        this->members[i] *= value;
    }
    return *this;
}

template <typename Type, SizeType SizeT>
inline Mat<Type, SizeT> Mat<Type, SizeT>::operator/(const Mat<Type, SizeT> &mat) const
{
    if ((sizeX * sizeY) != (mat.sizeX * mat.sizeY))
        throw std::invalid_argument("operator/: parameter must match the dimensions of this");
    Mat res(sizeX, sizeY);
    for (size_t i = 0; i < sizeX * sizeY; ++i)
    {
        res.members[i] = this->members[i] / mat.members[i];
    }
    return res;
}

template <typename Type, SizeType SizeT>
inline Mat<Type, SizeT> Mat<Type, SizeT>::operator/(copy_fast_t<Type> value) const noexcept
{
    Mat res(sizeX, sizeY);
    for (size_t i = 0; i < sizeX * sizeY; ++i)
    {
        res.members[i] = this->members[i] / value;
    }
    return res;
}

template <typename Type, SizeType SizeT>
inline Mat<Type, SizeT> Mat<Type, SizeT>::operator/=(const Mat<Type, SizeT> &mat)
{
    if ((sizeX * sizeY) != (mat.sizeX * mat.sizeY))
        throw std::invalid_argument("operator/=: parameter must match the dimensions of this");
    for (size_t i = 0; i < sizeX * sizeY; ++i)
    {
        this->members[i] /= mat.members[i];
    }
    return *this;
}

template <typename Type, SizeType SizeT>
inline Mat<Type, SizeT> Mat<Type, SizeT>::operator/=(copy_fast_t<Type> value) noexcept
{
    for (size_t i = 0; i < sizeX * sizeY; ++i)
    {
        this->members[i] /= value;
    }
    return *this;
}

template <typename Type, SizeType SizeT>
inline Mat<Type, SizeT>::~Mat() noexcept
{
    delete[] members;
}

using fMat = Mat<float>;
using dMat = Mat<double>;
using iMat = Mat<int>;

template <typename T>
concept Matrix = std::is_base_of_v<Mat<typename T::type, typename T::storage_type>, T>;
