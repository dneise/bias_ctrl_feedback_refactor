#ifndef FACTPP_STATS_H
#define FACTPP_STATS_H
/*
Header only library to calculate typical statistics like:
 * mean and
 * std(-deviation)
on a sample of observations, stored in a vector<T> of arbitrary numerical
objects.

For outlier-rich observations often estimations based on the CDF give
results closer to the truth. For these the:
 * median and the
 * cdf_std(-deviation)
are always given as well.
*/

#include <vector>
#include <algorithm>
#include <functional>

struct stats_t
{
    double median;
    double cdf_std;
    double mean;
    double std;
};

template<typename T>
stats_t calc_stats(std::vector<T> v, size_t N=0){
    if (N==0)
        N = v.size();

    double sum = 0;
    double square_sum = 0;

    for (size_t i=0; i<N; i++){
        sum += v[i];
        square_sum += v[i] * v[i];
    }


    std::vector<T> tmp(v);
    std::sort(tmp.begin(), tmp.begin()+N);

    double med = 0.;
    if (N%2){
        med = double(tmp[N/2]);
    }else{
        med = double(tmp[N/2 - 1] + tmp[N/2]) / 2.;
    }

    double right = double(tmp[size_t((0.5+0.341)*N)]);
    double left = double(tmp[size_t((0.5-0.341)*N)]);
    double cdf_std = (right - left) / 2.;

    stats_t result;
    result.median = med;
    result.cdf_std = cdf_std;

    result.mean = sum / N;

    square_sum /= N;
    square_sum -= result.mean * result.mean;
    result.std  = square_sum < 0 ? std::nan() : sqrt(square_sum);

    return result;
}


#endif  // FACTPP_STATS_H
