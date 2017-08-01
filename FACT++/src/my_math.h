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
