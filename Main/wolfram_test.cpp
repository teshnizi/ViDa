
#include <chrono>
#include "transmitter.h"


int main(){

    prepare_curl();

    auto start = chrono::system_clock::now();
    //here we calculate an integral:
    cout << endl << "integral of 7/t + 3(t^2)ln(t) over t from 1 to 10:\n" << integrate("7/t + 3(t^2)ln(t) over t from 1 to 10") << endl;
    auto finish = chrono::system_clock::now();
    chrono::duration<double> diff = finish - start;
    cout << "response time: " << diff.count() << endl << endl << endl;

    start = chrono::system_clock::now();
    string exp1 = "e^(-(x^2))";
    string exp2 = "e^(-(x^2))";
    //here we calculate a convolution:
    cout << "convolution of e^(-(x^2)) and e^(-(x^2)):\n" << convolve(exp1, exp2) << endl;
    finish = chrono::system_clock::now();
    diff = finish - start;
    cout << "response time: " << diff.count() << endl << endl;


    start = chrono::system_clock::now();
    exp1 = "theta(x) - theta(x-1)";
    exp2 = "theta(x) - theta(x-1)";
    cout << convolve(exp1, exp2) << endl;
    finish = chrono::system_clock::now();
    diff = finish - start;
    cout << "response time: " << diff.count() << endl << endl;

    cleanup();
}