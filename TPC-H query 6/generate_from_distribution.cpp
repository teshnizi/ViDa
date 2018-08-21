//
// Created by teshnizi on 12/08/18.
//
#include <cmath>
#include <iostream>
#include <stdio.h>

using namespace std;

const float pie = 3.14159265;
const float e = 2.71828182;

const float precision = 1000;


double Normal_CDF(double x);
double rational_approximation(double t) {
    // Abramowitz and Stegun formula 26.2.23.
    // The absolute value of the error should be less than 4.5 e-4.
    double c[] = {2.515517, 0.802853, 0.010328};
    double d[] = {1.432788, 0.189269, 0.001308};
    return t - ((c[2]*t + c[1])*t + c[0]) /
               (((d[2]*t + d[1])*t + d[0])*t + 1.0);
}
double Normal_CDF_Inverse(double p) {
    if (p <= 0.0 || p >= 1.0)
    {
        return -1;
    }

    // See article above for explanation of this section.
    if (p < 0.5)
        // F^-1(p) = - G^-1(p)
        return -rational_approximation( sqrt(-2.0*log(p)) );
    else
        // F^-1(p) = G^-1(1-p)
        return rational_approximation( sqrt(-2.0*log(1-p)) );
}

double Normal_CDF(double x) {
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
double Normal_pdf(float mean, float var, float x){
    return 1/(sqrt(var * 2 * pie)) * pow(e, -(x - mean) * (x - mean) / (2 * var));
}


//[m, M), time complexity: O(precision)
int normal_generate(float mean, float var, int table_size, float m, float M){
    float d_prob = Normal_CDF((m-mean)/sqrt(var));
    float u_prob = Normal_CDF((M-mean)/sqrt(var));
    int num = (u_prob - d_prob) * table_size;
    float expected_value = 0;

    float bucket_size = (M-m)/precision;

    for (float i = m; i < M; i += bucket_size) {
        expected_value += bucket_size * Normal_pdf(mean, var, i) * i;
    }
    return num * expected_value;
}

int uniform_generate(float a, float b, float table_size, float m, float M){
    int num = ((M - m )/(b - a)) * table_size;
    float mean = (M+m)/2;
    return num * mean;
}

//[m,M), time complexity: O(N)
int zipf_generate(int N, float s, int table_size, int m, int M){
    float H = 0;
    float ratio = 0;
    for (int i = 1; i <= N; ++i) {
        if (i == m) ratio -= H;
        if (i == M) ratio += H;
        H += 1.0 / (pow(i, s));
    }
    ratio /= H;
    int num = table_size * ratio;
    float expected_value = 0;
    for (int i = m; i < M; i++)
        expected_value += (1/(pow(i,s)))/H * i;
    return num * expected_value;
}


int main(){
    cout << normal_generate(0,1,1000,0,1) << endl;
    cout << uniform_generate(0,1,100,0,0.5) << endl;
    cout << zipf_generate(10, 2, 1000, 2, 3);
}