
#include <iostream>
#include <cmath>
#include <vector>
#include <time.h>
#include "tuples.h"

using namespace std;

const int test_size = 10000000;

const double discount_mean = 0.05;
const double discount_var = 0.0001;

const double extended_price_mean = 50;
const double extended_price_var = 10;

const double quantity_mean = 22;
const double quantity_var = 5;


double RationalApproximation(double t)
{
    // Abramowitz and Stegun formula 26.2.23.
    // The absolute value of the error should be less than 4.5 e-4.
    double c[] = {2.515517, 0.802853, 0.010328};
    double d[] = {1.432788, 0.189269, 0.001308};
    return t - ((c[2]*t + c[1])*t + c[0]) /
               (((d[2]*t + d[1])*t + d[0])*t + 1.0);
}

double NormalCDFInverse(double p)
{
    if (p <= 0.0 || p >= 1.0)
    {
        return -1;
    }

    // See article above for explanation of this section.
    if (p < 0.5)
        // F^-1(p) = - G^-1(p)
        return -RationalApproximation( sqrt(-2.0*log(p)) );
    else
        // F^-1(p) = G^-1(1-p)
        return RationalApproximation( sqrt(-2.0*log(1-p)) );
}


double CDF(double x)
{
    // constants
    double a1 =  0.254829592;
    double a2 = -0.284496736;
    double a3 =  1.421413741;
    double a4 = -1.453152027;
    double a5 =  1.061405429;
    double p  =  0.3275911;

    // Save the sign of x
    int sign = 1;
    if (x < 0)
        sign = -1;
    x = fabs(x)/sqrt(2.0);

    // A&S formula 7.1.26
    double t = 1.0/(1.0 + p*x);
    double y = 1.0 - (((((a5*t + a4)*t) + a3)*t + a2)*t + a1)*t*exp(-x*x);

    return 0.5*(1.0 + sign*y);
}
