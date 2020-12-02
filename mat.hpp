#pragma once
#include <stdexcept>
#include <string>
#include <ostream>
#include "generics.hpp"

template <typename Type = float, SizeType SizeT = size_t>
class Mat
{
private:
    Type *members = nullptr;
    SizeT sizeY = 0, sizeX = 0;
    size_t area = 0;

public:
    using type = Type;
    using storage_type = SizeT;

    Mat() = default;
    explicit Mat(SizeT, SizeT);
    explicit Mat(SizeT, SizeT, auto &&...);

    Mat(const Mat &);
    Mat(Mat &&);
    Mat operator=(const Mat &);
    Mat operator=(Mat &&);

    Type &At(SizeT, SizeT);
    Type *AtP(SizeT, SizeT);
    Type At(SizeT, SizeT) const;

    Type &FastAt(SizeT);
    Type FastAt(SizeT) const;
    Type *FastAtP(SizeT);

    SizeT SizeX() const;
    SizeT SizeY() const;
    size_t Area() const;

    Mat Dot(const Mat &) const;
    Mat Distribute(const Mat &) const;

    Mat operator+(const Mat &) const;
    Mat operator+(const Type &) const;
    Mat operator+=(const Mat &);
    Mat operator+=(const Type &);

    Mat operator-(const Mat &) const;
    Mat operator-(const Type &) const;
    Mat operator-=(const Mat &);
    Mat operator-=(const Type &);

    Mat operator*(const Mat &) const;
    Mat operator*(const Type &) const;
    Mat operator*=(const Mat &);
    Mat operator*=(const Type &);

    Mat operator/(const Mat &) const;
    Mat operator/(const Type &) const;
    Mat operator/=(const Mat &);
    Mat operator/=(const Type &);

    Type *begin() { return &members[0]; }
    Type *end() { return &members[area]; }

    friend std::ostream &operator<<(std::ostream &os, const Mat &mat)
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

    std::string Save() const
    {
        std::string res = "(";
        res += std::to_string(sizeX) + ',' + std::to_string(sizeY) + ',';
        for (SizeT i = 0; i < SizeY(); ++i)
        {
            for (SizeT j = 0; j < SizeX(); ++j)
            {
                res += std::to_string(this->At(j, i)) + ',';
            }
        }
        //removing last ,
        res.resize(res.size() - 1);
        res += ");";
        return res;
    }

    //Transform functions

    template <bool CORDS_PARAMS_FLAG = false>
    Mat Transform(const auto &Func) const
    {
        Mat res(sizeX, sizeY);
        for (SizeT i = 0; i < sizeY; ++i)
        {
            for (SizeT j = 0; j < sizeX; ++j)
            {
                if constexpr (CORDS_PARAMS_FLAG)
                {
                    res.At(j, i) = Func(this->At(j, i), j, i);
                }
                else
                {
                    res.At(j, i) = Func(this->At(j, i));
                }
            }
        }
        return res;
    }

    template <bool CORDS_PARAMS_FLAG = false>
    Mat Transform(const auto &Func, const Mat &mat) const
    {
        Mat res(sizeX, sizeY);
        for (SizeT i = 0; i < sizeY; ++i)
        {
            for (SizeT j = 0; j < sizeX; ++j)
            {
                if constexpr (CORDS_PARAMS_FLAG)
                {
                    res.At(j, i) = Func(this->At(j, i), mat.At(j, i), j, i);
                }
                else
                {
                    res.At(j, i) = Func(this->At(j, i), mat.At(j, i));
                }
            }
        }
        return res;
    }

    //ApplyFunction function

    template <bool CORDS_PARAMS_FLAG = false>
    void ApplyFunction(const auto &Func)
    {
        for (SizeT i = 0; i < sizeY; ++i)
        {
            for (SizeT j = 0; j < sizeX; ++j)
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
    void ApplyFunction(const auto &Func, const Mat &mat)
    {
        for (SizeT i = 0; i < sizeY; ++i)
        {
            for (SizeT j = 0; j < sizeX; ++j)
            {
                if constexpr (CORDS_PARAMS_FLAG)
                {
                    Func(this->members[i][j], mat.At(j, i), j, i);
                }
                else
                {
                    Func(this->members[i][j], mat.At(j, i));
                }
            }
        }
    }

    ~Mat();
};

template <typename Type, SizeType SizeT>
inline Mat<Type, SizeT>::Mat(SizeT w, SizeT h) : sizeX(w), sizeY(h), area(w * h)
{
    members = new Type[area]{};
}

template <typename Type, SizeType SizeT>
inline Mat<Type, SizeT>::Mat(SizeT w, SizeT h, auto &&... membs) : sizeX(w), sizeY(h), area(w * h)
{
    if (sizeof...(membs) > (area))
        throw std::invalid_argument("dimensions of matrix must match number of values");
    members = new Type[area]{membs...};
}

template <typename Type, SizeType SizeT>
inline Mat<Type, SizeT>::Mat(const Mat<Type, SizeT> &mat)
{
    sizeX = mat.sizeX;
    sizeY = mat.sizeY;
    area = mat.area;
    if (area)
    {
        members = new Type[area]{Type()};
    }
    for (size_t i = 0; i < mat.Area(); ++i)
    {
        FastAt(i) = mat.FastAt(i);
    }
}

template <typename Type, SizeType SizeT>
inline Mat<Type, SizeT>::Mat(Mat<Type, SizeT> &&mat)
{
    sizeX = mat.sizeX;
    sizeY = mat.sizeY;
    area = mat.area;
    members = mat.members;
    mat.members = nullptr;
    mat.sizeX = mat.sizeY = mat.area = 0;
}

template <typename Type, SizeType SizeT>
inline Mat<Type, SizeT> Mat<Type, SizeT>::operator=(const Mat<Type, SizeT> &mat)
{
    sizeX = mat.sizeX;
    sizeY = mat.sizeY;
    area = mat.area;
    if (members)
    {
        delete[] members;
        members = nullptr;
    }
    if (area)
    {
        members = new Type[area]{Type()};
    }
    for (size_t i = 0; i < area; ++i)
    {
        FastAt(i) = mat.FastAt(i);
    }
    return *this;
}

template <typename Type, SizeType SizeT>
inline Mat<Type, SizeT> Mat<Type, SizeT>::operator=(Mat<Type, SizeT> &&mat)
{
    sizeX = mat.sizeX;
    sizeY = mat.sizeY;
    area = mat.area;
    if (members)
    {
        delete[] members;
    }
    members = mat.members;
    mat.members = nullptr;
    mat.sizeX = mat.sizeY = mat.area = 0;
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
inline Type &Mat<Type, SizeT>::FastAt(SizeT index)
{
    if (index >= area)
        throw std::out_of_range("FastAt: index out of range");
    return members[index];
}

template <typename Type, SizeType SizeT>
inline Type Mat<Type, SizeT>::FastAt(SizeT index) const
{
    if (index >= area)
        throw std::out_of_range("FastAt: index out of range");
    return members[index];
}

template <typename Type, SizeType SizeT>
inline Type *Mat<Type, SizeT>::FastAtP(SizeT index)
{
    if (index >= area)
        throw std::out_of_range("FastAt: index out of range");
    return &members[index];
}

template <typename Type, SizeType SizeT>
inline SizeT Mat<Type, SizeT>::SizeX() const { return sizeX; }

template <typename Type, SizeType SizeT>
inline SizeT Mat<Type, SizeT>::SizeY() const { return sizeY; }

template <typename Type, SizeType SizeT>
inline size_t Mat<Type, SizeT>::Area() const { return area; }

template <typename Type, SizeType SizeT>
inline Mat<Type, SizeT> Mat<Type, SizeT>::Dot(const Mat<Type, SizeT> &mat) const
{
    if (!((sizeX == mat.sizeY) || (sizeY == mat.sizeX)))
        throw std::invalid_argument("Dot: dimensions invalid");
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
inline Mat<Type, SizeT> Mat<Type, SizeT>::Distribute(const Mat<Type, SizeT> &mat) const
{
    Mat res(sizeX, sizeY);
    for (size_t i = 0; i < area; ++i)
    {
        for (size_t j = 0; j < mat.area; ++j)
        {
            res.FastAt(i) += FastAt(i) * mat.FastAt(j);
        }
    }
    return res;
}

template <typename Type, SizeType SizeT>
inline Mat<Type, SizeT> Mat<Type, SizeT>::operator+(const Mat<Type, SizeT> &mat) const
{
    if (area != mat.area)
        throw std::invalid_argument("operator+: parameter must match the dimensions of this");
    Mat res(sizeX, sizeY);
    for (size_t i = 0; i < area; ++i)
    {
        res.members[i] = this->members[i] + mat.members[i];
    }
    return res;
}

template <typename Type, SizeType SizeT>
inline Mat<Type, SizeT> Mat<Type, SizeT>::operator+(const Type &value) const
{
    Mat res(sizeX, sizeY);
    for (size_t i = 0; i < area; ++i)
    {
        res.members[i] = this->members[i] + value;
    }
    return res;
}

template <typename Type, SizeType SizeT>
inline Mat<Type, SizeT> Mat<Type, SizeT>::operator+=(const Mat<Type, SizeT> &mat)
{
    if (area != mat.area)
        throw std::invalid_argument("operator+=: parameter must match the dimensions of this");
    for (size_t i = 0; i < area; ++i)
    {
        this->members[i] += mat.members[i];
    }
}

template <typename Type, SizeType SizeT>
inline Mat<Type, SizeT> Mat<Type, SizeT>::operator+=(const Type &value)
{
    for (size_t i = 0; i < area; ++i)
    {
        this->members[i] += value;
    }
    return *this;
}

template <typename Type, SizeType SizeT>
inline Mat<Type, SizeT> Mat<Type, SizeT>::operator-(const Mat<Type, SizeT> &mat) const
{
    if (area != mat.area)
        throw std::invalid_argument("operator-: parameter must match the dimensions of this");
    Mat res(sizeX, sizeY);
    for (size_t i = 0; i < area; ++i)
    {
        res.members[i] = this->members[i] - mat.members[i];
    }
    return res;
}

template <typename Type, SizeType SizeT>
inline Mat<Type, SizeT> Mat<Type, SizeT>::operator-(const Type &value) const
{
    Mat res(sizeX, sizeY);
    for (size_t i = 0; i < area; ++i)
    {
        res.members[i] = this->members[i] - value;
    }
    return res;
}

template <typename Type, SizeType SizeT>
inline Mat<Type, SizeT> Mat<Type, SizeT>::operator-=(const Mat<Type, SizeT> &mat)
{
    if (area != mat.area)
        throw std::invalid_argument("operator-=: parameter must match the dimensions of this");
    for (size_t i = 0; i < area; ++i)
    {
        this->members[i] -= mat.members[i];
    }
    return *this;
}

template <typename Type, SizeType SizeT>
inline Mat<Type, SizeT> Mat<Type, SizeT>::operator-=(const Type &value)
{
    for (size_t i = 0; i < area; ++i)
    {
        this->members[i] -= value;
    }
    return *this;
}

template <typename Type, SizeType SizeT>
inline Mat<Type, SizeT> Mat<Type, SizeT>::operator*(const Mat<Type, SizeT> &mat) const
{
    if (area != mat.area)
        throw std::invalid_argument("operator*: parameter must match the dimensions of this");
    Mat res(sizeX, sizeY);
    for (size_t i = 0; i < area; ++i)
    {
        res.members[i] = this->members[i] * mat.members[i];
    }
    return res;
}

template <typename Type, SizeType SizeT>
inline Mat<Type, SizeT> Mat<Type, SizeT>::operator*(const Type &value) const
{
    Mat res(sizeX, sizeY);
    for (size_t i = 0; i < area; ++i)
    {
        res.members[i] = this->members[i] * value;
    }
    return res;
}

template <typename Type, SizeType SizeT>
inline Mat<Type, SizeT> Mat<Type, SizeT>::operator*=(const Mat<Type, SizeT> &mat)
{
    if (area != mat.area)
        throw std::invalid_argument("operator*=: parameter must match the dimensions of this");
    for (size_t i = 0; i < area; ++i)
    {
        this->members[i] *= mat.members[i];
    }
    return *this;
}

template <typename Type, SizeType SizeT>
inline Mat<Type, SizeT> Mat<Type, SizeT>::operator*=(const Type &value)
{
    for (size_t i = 0; i < area; ++i)
    {
        this->members[i] *= value;
    }
    return *this;
}

template <typename Type, SizeType SizeT>
inline Mat<Type, SizeT> Mat<Type, SizeT>::operator/(const Mat<Type, SizeT> &mat) const
{
    if (area != mat.area)
        throw std::invalid_argument("operator/: parameter must match the dimensions of this");
    Mat res(sizeX, sizeY);
    for (size_t i = 0; i < area; ++i)
    {
        res.members[i] = this->members[i] / mat.members[i];
    }
    return res;
}

template <typename Type, SizeType SizeT>
inline Mat<Type, SizeT> Mat<Type, SizeT>::operator/(const Type &value) const
{
    Mat res(sizeX, sizeY);
    for (size_t i = 0; i < area; ++i)
    {
        res.members[i] = this->members[i] / value;
    }
    return res;
}

template <typename Type, SizeType SizeT>
inline Mat<Type, SizeT> Mat<Type, SizeT>::operator/=(const Mat<Type, SizeT> &mat)
{
    if (area != mat.area)
        throw std::invalid_argument("operator/=: parameter must match the dimensions of this");
    for (size_t i = 0; i < area; ++i)
    {
        this->members[i] /= mat.members[i];
    }
    return *this;
}

template <typename Type, SizeType SizeT>
inline Mat<Type, SizeT> Mat<Type, SizeT>::operator/=(const Type &value)
{
    for (size_t i = 0; i < area; ++i)
    {
        this->members[i] /= value;
    }
    return *this;
}

template <typename Type, SizeType SizeT>
inline Mat<Type, SizeT>::~Mat()
{
    delete[] members;
}

using fMat = Mat<float>;
using dMat = Mat<double>;
using iMat = Mat<int>;
using MLMat = Mat<float, uint16_t>;
using ImgMat = Mat<uint8_t, uint16_t>;

template <typename T>
concept Matrix = std::is_base_of<Mat<typename T::type, typename T::storage_type>, T>::value;
