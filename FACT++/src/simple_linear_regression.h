#ifndef SIMPLE_LINEAR_REGRESSION
#define SIMPLE_LINEAR_REGRESSION

struct Fit_t
{
    double intercept;
    double slope;

    Fit_t(i, s):
        intercept(i),
        slope(s)
        {}
};


Fit_t linear_regression(T* X, S* Y, int N){

    double x  = 0;
    double y  = 0;
    double xx = 0;
    double xy = 0;


    for (int i=0; i<=N; i++)
    {
        x  += X[i];
        xx += X[i] * X[i];
        y  += Y[i];
        xy += X[i] * Y[i];
    }

    const double m1 = xy - x*y / N;
    const double m2 = xx - x*x / N;

    const double m = m2!=0 ? m1/m2 : 0;

    const double t = (y - m*x) / N;

    return Fit_t(t, m);
}

#endif
