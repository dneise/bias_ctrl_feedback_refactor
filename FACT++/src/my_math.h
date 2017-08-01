#include <vector>

template<typename T>
double median(std::vector<T> v, size_t N=0)
{
    if (N==0)
        N = v.size();

    std::vector<T> tmp(v);
    std::sort(tmp.begin(), tmp.begin()+N);

    if (N%2){
        return double(tmp[N/2]);
    }else{
        return double(tmp[N/2 - 1] + tmp[N/2]) / 2.;
    }

}

template<typename T>
double cdf_std_dev(std::vector<T> v, size_t N=0){
    if (N==0)
        N = v.size();

    double m = median(v);
    std::vector<double> tmp(v.size(), 0.);
    for (size_t i=0; i<tmp.size(); i++){
        tmp[i] = fabs(v[i] - m);
    }
    std::sort(tmp.begin(), tmp.begin()+N);

    return double(tmp[size_t(0.6827*N)]);
}

struct median_and_std_t
{
    double median;
    double std;
};

template<typename T>
median_and_std_t median_and_std(std::vector<T> v, size_t N=0){
    if (N==0)
        N = v.size();

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
    double std = (right - left) / 2.;

    median_and_std_t result;
    result.median = med;
    result.std = std;
    return result;
}


