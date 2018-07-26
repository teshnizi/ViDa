
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

vector <double> generate_data_based_on_normal_distribution(double mean, double var, double num, double min, double max, unsigned seed = time(NULL)){

    vector <double> ret;

    srand(seed);
    min -= mean;
    max -= mean;
    min /= sqrt(var);
    max /= sqrt(var);

    double d_prob = CDF(min);
    double u_prob = CDF(max);
//    cout<< d_prob << " " << u_prob <<endl;

    for (int i = 0; i < num; ++i) {
        double k = (rand()%10000)/10000.0;
        k *= u_prob - d_prob;
        k += d_prob;
        k = NormalCDFInverse(k);
        k *= sqrt(var);
        k += mean;
        ret.push_back(k);
    }
    return ret;
}

vector <line_item> execute_query(int min_ship_date, int max_ship_date,
                                        double min_price, double max_price,
                                        double min_discount, double max_discount,
                                        double min_quantity, double max_quantity){

    //assume that date are uniformly distributed from year 1990 to 2020
    double day_ratio = (max_ship_date - min_ship_date) / (30.0 * 365.0);
    double discount_ratio = CDF((max_discount-discount_mean)/sqrt(discount_var)) - CDF((min_discount-discount_mean)/sqrt(discount_var));
    double quantity_ratio = CDF((max_quantity-quantity_mean)/sqrt(quantity_var)) - CDF((min_quantity-quantity_mean)/sqrt(quantity_var));
    double price_ratio = CDF((max_price-extended_price_mean)/sqrt(extended_price_var)) - CDF((min_price-extended_price_mean)/sqrt(extended_price_var));

    int num = (double)test_size * day_ratio * discount_ratio * quantity_ratio * price_ratio;

    //num shows that how many rows should we generate.

    vector <int> dates;
    int interval = max_ship_date - min_ship_date;

    for (int i = 0; i < num; ++i)
        dates.push_back(min_ship_date + ((rand()%interval)));

    vector <double> discounts = generate_data_based_on_normal_distribution(discount_mean, discount_var, num,
                                                                            min_discount, max_discount);
    vector <double> quantities = generate_data_based_on_normal_distribution(quantity_mean, quantity_var, num,
                                                                            min_quantity, max_quantity);
    vector <double> prices = generate_data_based_on_normal_distribution(extended_price_mean, extended_price_var, num,
                                                                            min_price, max_price);

    vector <line_item> ret;
    for (int i = 0; i < num; ++i) {
        ret.push_back(line_item(i, dates[i], prices[i], discounts[i], quantities[i]));
    }

    return ret;
}