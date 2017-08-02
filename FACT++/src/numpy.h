#ifndef FACTPP_NUMPY
#define FACTPP_NUMPY

/*
This lib provides elementwise vector operations, similar to python-numpy.

These functions are provided at the moment:

sum = a + b
difference = a - b
product = a * b
quotient = a / b
scale_vector_a = c * a
power = a ^ b
is_smaller = a < b
e_function = exp(a)

*/

#include <vector>
#include <algorithm>
#include <functional>


template <typename T>
std::vector<T> operator+(const std::vector<T>& a, const std::vector<T>& b)
{
    assert(a.size() == b.size());

    std::vector<T> result;
    result.reserve(a.size());

    std::transform(a.begin(), a.end(), b.begin(),
                   std::back_inserter(result), std::plus<T>());
    return result;
}

template <typename T>
std::vector<T> operator+(const std::vector<T>& a, T b)
{
    std::vector<T> result;
    result.reserve(a.size());

    std::transform(a.begin(), a.end(), result.begin(),
                    std::bind1st(std::plus<T>(), b));
    return result;
}

template <typename T>
std::vector<T> operator-(const std::vector<T>& a, const std::vector<T>& b)
{
    assert(a.size() == b.size());

    std::vector<T> result;
    result.reserve(a.size());

    std::transform(a.begin(), a.end(), b.begin(),
                   std::back_inserter(result), std::minus<T>());
    return result;
}

template <typename T>
std::vector<T> operator*(const std::vector<T>& a, const std::vector<T>& b)
{
    assert(a.size() == b.size());

    std::vector<T> result;
    result.reserve(a.size());

    std::transform(a.begin(), a.end(), b.begin(),
                   std::back_inserter(result), std::multiplies<T>());
    return result;
}

template <typename T>
std::vector<T> operator/(const std::vector<T>& a, const std::vector<T>& b)
{
    assert(a.size() == b.size());

    std::vector<T> result;
    result.reserve(a.size());

    std::transform(a.begin(), a.end(), b.begin(),
                   std::back_inserter(result), std::devides<T>());
    return result;
}

template <typename T>
std::vector<T> operator*(const std::vector<T>& a, T b)
{
    std::vector<T> result;
    result.reserve(a.size());

    std::transform(a.begin(), a.end(), result.begin(),
                   std::bind1st(std::multiplies<T>(), b));
    return result;
}

template <typename T>
std::vector<T> operator^(const std::vector<T>& a, const std::vector<T>& b)
{
    assert(a.size() == b.size());

    std::vector<T> result;
    result.reserve(a.size());
    for(size_t i=0; i<result.size(); i++){
        result[i] = pow(a[i], b[i]);
    }

    return result;
}

template <typename T>
std::vector<T> exp(const std::vector<T>& a)
{
    std::vector<T> result;
    result.reserve(a.size());
    for(size_t i=0; i<result.size(); i++){
        result[i] = exp(a[i]);
    }

    return result;
}

template <typename T>
std::vector<bool> operator<(const std::vector<T>& a, const std::vector<T>& b)
{
    assert(a.size() == b.size());

    std::vector<T> result;
    result.reserve(a.size());

    std::transform(a.begin(), a.end(), b.begin(),
                   std::back_inserter(result), std::less<T>());
    return result;
}

template <typename T>
std::vector<bool> operator>=(const std::vector<T>& a, const std::vector<T>& b)
{
    assert(a.size() == b.size());

    std::vector<T> result;
    result.reserve(a.size());

    std::transform(a.begin(), a.end(), b.begin(),
                   std::back_inserter(result), std::greater_equal<T>());
    return result;
}



template <typename T, typename U>
std::vector<T> set_where(const std::vector<T>& origin, const std::vector<S>& where, T val)
{
    assert(a.size() == b.size());

    std::vector<T> result;
    result.reserve(a.size());
    for(size_t i=0; i<result.size(); i++){
        if(where[i]){
            result[i] = val;
        }else{
            result[i] = a[i];
        }
    }

    return result;
}

#define  // FACTPP_NUMPY
