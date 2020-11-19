#pragma once
#include <stdexcept>
#include <string>
#include <ostream>

#ifdef __NVCC__
#include "cuda_mat.cu"
#else
#include "../../MiscHeaderFiles-master/generics.h"
#endif

#ifdef __NVCC__
template <typename Type, typename SizeT = size_t>
#else
template <typename Type, SizeType SizeT = size_t>
#endif
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

    template <typename... Params>
    explicit Mat(SizeT w, SizeT h, Params &&... membs) : sizeX(w), sizeY(h), area(w * h)
    {
        if (sizeof...(membs) > (area))
            throw std::invalid_argument("dimensions of matrix must match number of values");
        members = new Type[area]{membs...};
    }

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
    template <typename T, bool CORDS_PARAMS_FLAG = false>
    Mat Transform(const T &Func) const
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

    template <typename T, bool CORDS_PARAMS_FLAG = false>
    Mat Transform(const T &Func, const Mat &mat) const
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
    template <typename T, bool CORDS_PARAMS_FLAG = false>
    void ApplyFunction(const T &Func)
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

    template <typename T, bool CORDS_PARAMS_FLAG = false>
    void ApplyFunction(const T &Func, const Mat &mat)
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

#ifdef __NVCC__
template <typename Type, typename SizeT>
#else
template <typename Type, SizeType SizeT>
#endif
inline Mat<Type, SizeT>::Mat(SizeT w, SizeT h) : sizeX(w), sizeY(h), area(w * h)
{
    members = new Type[area]{};
}

#ifdef __NVCC__
template <typename Type, typename SizeT>
#else
template <typename Type, SizeType SizeT>
#endif
inline Mat<Type, SizeT>::Mat(const Mat<Type, SizeT> &mat)
{
    if (mat.members)
    {
        sizeX = mat.sizeX;
        sizeY = mat.sizeY;
        area = mat.area;
        members = new Type[area]{Type()};
        for (size_t i = 0; i < mat.Area(); ++i)
        {
            FastAt(i) = mat.FastAt(i);
        }
    }
}

#ifdef __NVCC__
template <typename Type, typename SizeT>
#else
template <typename Type, SizeType SizeT>
#endif
inline Mat<Type, SizeT>::Mat(Mat<Type, SizeT> &&mat)
{
    sizeX = mat.sizeX;
    sizeY = mat.sizeY;
    area = mat.area;
    members = mat.members;
    mat.members = nullptr;
    mat.sizeX = mat.sizeY = mat.area = 0;
}

#ifdef __NVCC__
template <typename Type, typename SizeT>
#else
template <typename Type, SizeType SizeT>
#endif
inline Mat<Type, SizeT> Mat<Type, SizeT>::operator=(const Mat<Type, SizeT> &mat)
{
    if (mat.members)
    {
        sizeX = mat.sizeX;
        sizeY = mat.sizeY;
        area = mat.area;
        members = new Type[area]{Type()};
        for (size_t i = 0; i < mat.Area(); ++i)
        {
            FastAt(i) = mat.FastAt(i);
        }
    }
    return *this;
}

#ifdef __NVCC__
template <typename Type, typename SizeT>
#else
template <typename Type, SizeType SizeT>
#endif
inline Mat<Type, SizeT> Mat<Type, SizeT>::operator=(Mat<Type, SizeT> &&mat)
{
    sizeX = mat.sizeX;
    sizeY = mat.sizeY;
    area = mat.area;
    members = mat.members;
    mat.members = nullptr;
    mat.sizeX = mat.sizeY = mat.area = 0;
    return *this;
}

#ifdef __NVCC__
template <typename Type, typename SizeT>
#else
template <typename Type, SizeType SizeT>
#endif
inline Type &Mat<Type, SizeT>::At(SizeT x, SizeT y) //indexing matrix
{
    if ((x >= sizeX) || (y >= sizeY))
        throw std::out_of_range("At: out of range, " + std::to_string(x) + ' ' + std::to_string(y));
    return members[(y * sizeX) + x];
}

#ifdef __NVCC__
template <typename Type, typename SizeT>
#else
template <typename Type, SizeType SizeT>
#endif
inline Type *Mat<Type, SizeT>::AtP(SizeT x, SizeT y)
{
    if ((x >= sizeX) || (y >= sizeY))
        throw std::out_of_range("At: out of range, " + std::to_string(x) + ' ' + std::to_string(y));
    return &members[(y * sizeX) + x];
}

#ifdef __NVCC__
template <typename Type, typename SizeT>
#else
template <typename Type, SizeType SizeT>
#endif
inline Type Mat<Type, SizeT>::At(SizeT x, SizeT y) const //indexing matrix
{
    if ((x >= sizeX) || (y >= sizeY))
        throw std::out_of_range("At: out of range, " + std::to_string(x) + ' ' + std::to_string(y));
    return members[(y * sizeX) + x];
}

#ifdef __NVCC__
template <typename Type, typename SizeT>
#else
template <typename Type, SizeType SizeT>
#endif
inline Type &Mat<Type, SizeT>::FastAt(SizeT index)
{
    if (index >= area)
        throw std::out_of_range("FastAt: index out of range");
    return members[index];
}

#ifdef __NVCC__
template <typename Type, typename SizeT>
#else
template <typename Type, SizeType SizeT>
#endif
inline Type Mat<Type, SizeT>::FastAt(SizeT index) const
{
    if (index >= area)
        throw std::out_of_range("FastAt: index out of range");
    return members[index];
}

#ifdef __NVCC__
template <typename Type, typename SizeT>
#else
template <typename Type, SizeType SizeT>
#endif
inline Type *Mat<Type, SizeT>::FastAtP(SizeT index)
{
    if (index >= area)
        throw std::out_of_range("FastAt: index out of range");
    return &members[index];
}

#ifdef __NVCC__
template <typename Type, typename SizeT>
#else
template <typename Type, SizeType SizeT>
#endif
inline SizeT Mat<Type, SizeT>::SizeX() const
{
    return sizeX;
}

#ifdef __NVCC__
template <typename Type, typename SizeT>
#else
template <typename Type, SizeType SizeT>
#endif
inline SizeT Mat<Type, SizeT>::SizeY() const
{
    return sizeY;
}

#ifdef __NVCC__
template <typename Type, typename SizeT>
#else
template <typename Type, SizeType SizeT>
#endif
inline size_t Mat<Type, SizeT>::Area() const
{
    return area;
}

#ifdef __NVCC__
template <typename Type, typename SizeT>
#else
template <typename Type, SizeType SizeT>
#endif
inline Mat<Type, SizeT> Mat<Type, SizeT>::Dot(const Mat<Type, SizeT> &mat) const
{
    if (!((sizeX == mat.sizeY) || (sizeY == mat.sizeX)))
        throw std::invalid_argument("Dot: dimensions invalid");
    if (sizeX == mat.sizeY)
    {
        //i = y
        //j = x
        Mat res(mat.sizeX, sizeY);
#ifndef __NVCC__
        for (SizeT i = 0; i < sizeY; ++i)
        {
            for (SizeT j = 0; j < mat.sizeX; ++j)
            {
                for (int k = 0; k < sizeX; ++k)
                {
                    res.FastAt(j + (i * mat.sizeX)) += FastAt(k + (i * sizeX)) * mat.FastAt(j + (k * mat.sizeX));
                }
            }
        }
#else
        mat_matrix_mult(this->members, mat.members, res.members, sizeX, sizeY, mat.sizeX, mat.sizeY, mat.sizeX, sizeY);
#endif
        return res;
    }
    return mat.Dot(*this);
}

#ifdef __NVCC__
template <typename Type, typename SizeT>
#else
template <typename Type, SizeType SizeT>
#endif
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

#ifdef __NVCC__
template <typename Type, typename SizeT>
#else
template <typename Type, SizeType SizeT>
#endif
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

#ifdef __NVCC__
template <typename Type, typename SizeT>
#else
template <typename Type, SizeType SizeT>
#endif
inline Mat<Type, SizeT> Mat<Type, SizeT>::operator+(const Type &value) const
{
    Mat res(sizeX, sizeY);
    for (size_t i = 0; i < area; ++i)
    {
        res.members[i] = this->members[i] + value;
    }
    return res;
}

#ifdef __NVCC__
template <typename Type, typename SizeT>
#else
template <typename Type, SizeType SizeT>
#endif
inline Mat<Type, SizeT> Mat<Type, SizeT>::operator+=(const Mat<Type, SizeT> &mat)
{
    if (area != mat.area)
        throw std::invalid_argument("operator+=: parameter must match the dimensions of this");
    for (size_t i = 0; i < area; ++i)
    {
        this->members[i] += mat.members[i];
    }
}

#ifdef __NVCC__
template <typename Type, typename SizeT>
#else
template <typename Type, SizeType SizeT>
#endif
inline Mat<Type, SizeT> Mat<Type, SizeT>::operator+=(const Type &value)
{
    for (size_t i = 0; i < area; ++i)
    {
        this->members[i] += value;
    }
    return *this;
}

#ifdef __NVCC__
template <typename Type, typename SizeT>
#else
template <typename Type, SizeType SizeT>
#endif
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

#ifdef __NVCC__
template <typename Type, typename SizeT>
#else
template <typename Type, SizeType SizeT>
#endif
inline Mat<Type, SizeT> Mat<Type, SizeT>::operator-(const Type &value) const
{
    Mat res(sizeX, sizeY);
    for (size_t i = 0; i < area; ++i)
    {
        res.members[i] = this->members[i] - value;
    }
    return res;
}

#ifdef __NVCC__
template <typename Type, typename SizeT>
#else
template <typename Type, SizeType SizeT>
#endif
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

#ifdef __NVCC__
template <typename Type, typename SizeT>
#else
template <typename Type, SizeType SizeT>
#endif
inline Mat<Type, SizeT> Mat<Type, SizeT>::operator-=(const Type &value)
{
    for (size_t i = 0; i < area; ++i)
    {
        this->members[i] -= value;
    }
    return *this;
}

#ifdef __NVCC__
template <typename Type, typename SizeT>
#else
template <typename Type, SizeType SizeT>
#endif
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

#ifdef __NVCC__
template <typename Type, typename SizeT>
#else
template <typename Type, SizeType SizeT>
#endif
inline Mat<Type, SizeT> Mat<Type, SizeT>::operator*(const Type &value) const
{
    Mat res(sizeX, sizeY);
    for (size_t i = 0; i < area; ++i)
    {
        res.members[i] = this->members[i] * value;
    }
    return res;
}

#ifdef __NVCC__
template <typename Type, typename SizeT>
#else
template <typename Type, SizeType SizeT>
#endif
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

#ifdef __NVCC__
template <typename Type, typename SizeT>
#else
template <typename Type, SizeType SizeT>
#endif
inline Mat<Type, SizeT> Mat<Type, SizeT>::operator*=(const Type &value)
{
    for (size_t i = 0; i < area; ++i)
    {
        this->members[i] *= value;
    }
    return *this;
}

#ifdef __NVCC__
template <typename Type, typename SizeT>
#else
template <typename Type, SizeType SizeT>
#endif
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

#ifdef __NVCC__
template <typename Type, typename SizeT>
#else
template <typename Type, SizeType SizeT>
#endif
inline Mat<Type, SizeT> Mat<Type, SizeT>::operator/(const Type &value) const
{
    Mat res(sizeX, sizeY);
    for (size_t i = 0; i < area; ++i)
    {
        res.members[i] = this->members[i] / value;
    }
    return res;
}

#ifdef __NVCC__
template <typename Type, typename SizeT>
#else
template <typename Type, SizeType SizeT>
#endif
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

#ifdef __NVCC__
template <typename Type, typename SizeT>
#else
template <typename Type, SizeType SizeT>
#endif
inline Mat<Type, SizeT> Mat<Type, SizeT>::operator/=(const Type &value)
{
    for (size_t i = 0; i < area; ++i)
    {
        this->members[i] /= value;
    }
    return *this;
}

#ifdef __NVCC__
template <typename Type, typename SizeT>
#else
template <typename Type, SizeType SizeT>
#endif
inline Mat<Type, SizeT>::~Mat()
{
    delete[] members;
}

using fMat = Mat<float>;
using dMat = Mat<double>;
using iMat = Mat<int>;
using MLMat = Mat<float, uint16_t>;
using ImgMat = Mat<uint8_t, uint16_t>;

#ifndef __NVCC__
template <typename T>
concept Matrix = std::is_base_of<Mat<typename T::type, typename T::storage_type>, T>::value;
#endif