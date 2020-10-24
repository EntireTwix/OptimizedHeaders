#pragma once
#include <stdexcept>
#include <string>
#include <ostream>

template <typename Type = float>
class Mat
{
private:
    Type *members = nullptr;
    size_t sizeY = 0, sizeX = 0;

public:
    using type = Type;

    Mat() = default;
    explicit Mat(size_t x, size_t y);

    template <typename... Params>
    explicit Mat(size_t w, size_t h, Params... membs) : sizeX(w), sizeY(h)
    {
        if (sizeof...(membs) != (w * h))
            throw std::invalid_argument("dimensions of matrix must match number of values");
        members = new Type[w * h]{membs...};
    }

    Mat(const Mat &mat);
    Mat(Mat &&mat);
    Mat operator=(const Mat &mat);
    Mat operator=(Mat &&mat);

    Type &At(size_t x, size_t y);
    Type *AtP(size_t x, size_t y);
    Type At(size_t x, size_t y) const;

    size_t SizeX() const;
    size_t SizeY() const;
    size_t Area() const;

    Mat Dot(const Mat &mat) const;
    Mat Transpose() const;

    Mat operator+(const Mat &mat) const;
    Mat operator-(const Mat &mat) const;
    Mat operator*(const Mat &mat) const;
    Mat operator/(const Mat &mat) const;

    Type *begin()
    {
        return &members[0];
    }
    Type *end()
    {
        return &members[sizeX * sizeY];
    }

    friend std::ostream &operator<<(std::ostream &os, const Mat &mat)
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

    std::string Save() const
    {
        std::string res = "(";
        res += std::to_string(sizeX) + ',' + std::to_string(sizeY) + ',';
        for (size_t i = 0; i < SizeY(); ++i)
        {
            for (size_t j = 0; j < SizeX(); ++j)
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
    template <typename Function, bool CORDS_PARAMS_FLAG = false>
    Mat Transform(const Function &Func) const
    {
        Mat res(sizeX, sizeY);
        for (size_t i = 0; i < sizeY; ++i)
        {
            for (size_t j = 0; j < sizeX; ++j)
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
    template <typename Function, bool CORDS_PARAMS_FLAG = false>
    Mat Transform(const Function &Func, const Mat &mat) const
    {
        Mat res(sizeX, sizeY);
        for (size_t i = 0; i < sizeY; ++i)
        {
            for (size_t j = 0; j < sizeX; ++j)
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
    template <typename Function, bool CORDS_PARAMS_FLAG = false>
    void ApplyFunction(const Function &Func)
    {
        for (size_t i = 0; i < sizeY; ++i)
        {
            for (size_t j = 0; j < sizeX; ++j)
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
    template <typename Function, bool CORDS_PARAMS_FLAG = false>
    void ApplyFunction(const Function &Func, const Mat &mat)
    {
        for (size_t i = 0; i < sizeY; ++i)
        {
            for (size_t j = 0; j < sizeX; ++j)
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

template <typename Type>
inline Mat<Type>::Mat(size_t x, size_t y) : sizeX(x), sizeY(y)
{
    members = new Type[sizeX * sizeY]{Type()};
}

template <typename Type>
inline Mat<Type>::Mat(const Mat<Type> &mat)
{
    sizeX = mat.sizeX;
    sizeY = mat.sizeY;
    members = new Type[sizeX * sizeY]{Type()};

    for (size_t i = 0; i < sizeY; ++i)
    {
        for (size_t j = 0; j < sizeX; ++j)
        {
            At(j, i) = mat.At(j, i);
        }
    }
}

template <typename Type>
inline Mat<Type>::Mat(Mat<Type> &&mat)
{
    sizeX = mat.sizeX;
    sizeY = mat.sizeY;
    members = mat.members;
    mat.members = nullptr;
    mat.sizeX = mat.sizeY = 0;
}

template <typename Type>
inline Mat<Type> Mat<Type>::operator=(const Mat<Type> &mat)
{
    sizeX = mat.sizeX;
    sizeY = mat.sizeY;
    members = new Type[sizeX * sizeY];
    for (size_t i = 0; i < sizeY; ++i)
    {
        for (size_t j = 0; j < sizeX; ++j)
        {
            At(j, i) = mat.At(j, i);
        }
    }
    return *this;
}

template <typename Type>
inline Mat<Type> Mat<Type>::operator=(Mat<Type> &&mat)
{
    sizeX = mat.sizeX;
    sizeY = mat.sizeY;
    members = mat.members;
    mat.members = nullptr;
    mat.sizeX = mat.sizeY = 0;
    return *this;
}

template <typename Type>
inline Type &Mat<Type>::At(size_t x, size_t y) //indexing matrix
{
    if ((x >= sizeX) || (y >= sizeY))
        throw std::out_of_range("At: out of range, " + std::to_string(x) + ' ' + std::to_string(y));
    return members[(y * sizeX) + x];
}

template <typename Type>
inline Type *Mat<Type>::AtP(size_t x, size_t y)
{
    if ((x >= sizeX) || (y >= sizeY))
        throw std::out_of_range("At: out of range, " + std::to_string(x) + ' ' + std::to_string(y));
    return &members[(y * sizeX) + x];
}

template <typename Type>
inline Type Mat<Type>::At(size_t x, size_t y) const //indexing matrix
{
    if ((x >= sizeX) || (y >= sizeY))
        throw std::out_of_range("At: out of range, " + std::to_string(x) + ' ' + std::to_string(y));
    return members[(y * sizeX) + x];
}

template <typename Type>
inline size_t Mat<Type>::SizeX() const
{
    return sizeX;
}

template <typename Type>
inline size_t Mat<Type>::SizeY() const
{
    return sizeY;
}

template <typename Type>
inline size_t Mat<Type>::Area() const
{
    return sizeY * sizeX;
}

template <typename Type>
inline Mat<Type> Mat<Type>::Dot(const Mat<Type> &mat) const
{
    if (sizeY != 1)
        throw std::invalid_argument("Dot: internal must be a vector");
    if (sizeX != mat.sizeY)
        throw std::invalid_argument("Dot: length of vector internal must match height of mat arg");

    Mat res(mat.sizeX, sizeY); //product dimensions are the x of both mats
    Type amount;
    for (size_t i = 0; i < sizeY; ++i) //for each y on the A matrix
    {
        for (size_t j = 0; j < mat.sizeX; ++j) //for each x on the B matrix
        {
            amount = 0;
            for (int k = 0; k < sizeX; ++k) //is equivelant to mat.sizeY()
                amount += At(k, i) * mat.At(j, k);
            res.At(j, i) = amount;
        }
    }
    return res;
}

template <typename Type>
inline Mat<Type> Mat<Type>::Transpose() const
{
    Mat res(sizeY, sizeX);
    for (size_t i = 0; i < sizeY; ++i)
    {
        for (size_t j = 0; j < sizeX; ++j)
        {
            res.At(i, j) = At(j, i);
        }
    }
    return res;
}

template <typename Type>
inline Mat<Type> Mat<Type>::operator+(const Mat<Type> &mat) const
{
    if ((mat.SizeX() != SizeX()) || (mat.SizeY() != SizeY()))
        throw std::invalid_argument("operator+: parameter must match the dimensions of this");
    Mat res(SizeX(), SizeY());
    for (size_t i = 0; i < sizeY; ++i)
    {
        for (size_t j = 0; j < sizeX; ++j)
        {
            res.At(j, i) = At(j, i) + mat.At(j, i);
        }
    }
    return res;
}

template <typename Type>
inline Mat<Type> Mat<Type>::operator-(const Mat<Type> &mat) const
{
    if ((mat.SizeX() != SizeX()) || (mat.SizeY() != SizeY()))
        throw std::invalid_argument("operator-: parameter must match the dimensions of this");
    Mat res(SizeX(), SizeY());
    for (size_t i = 0; i < sizeY; ++i)
    {
        for (size_t j = 0; j < sizeX; ++j)
        {
            res.At(j, i) = At(j, i) - mat.At(j, i);
        }
    }
    return res;
}

template <typename Type>
inline Mat<Type> Mat<Type>::operator*(const Mat<Type> &mat) const
{
    if ((mat.SizeX() != SizeX()) || (mat.SizeY() != SizeY()))
        throw std::invalid_argument("operator*: parameter must match the dimensions of this");
    Mat res(SizeX(), SizeY());
    for (size_t i = 0; i < sizeY; ++i)
    {
        for (size_t j = 0; j < sizeX; ++j)
        {
            res.At(j, i) = At(j, i) * mat.At(j, i);
        }
    }
    return res;
}

template <typename Type>
inline Mat<Type> Mat<Type>::operator/(const Mat<Type> &mat) const
{
    if ((mat.SizeX() != SizeX()) || (mat.SizeY() != SizeY()))
        throw std::invalid_argument("operator*: parameter must match the dimensions of this");
    Mat res(SizeX(), SizeY());
    for (size_t i = 0; i < sizeY; ++i)
    {
        for (size_t j = 0; j < sizeX; ++j)
        {
            res.At(j, i) = At(j, i) * mat.At(j, i);
        }
    }
    return res;
}

template <typename Type>
inline Mat<Type>::~Mat()
{
    delete[] members;
}

using fMat = Mat<float>;
using dMat = Mat<double>;
using iMat = Mat<int>;