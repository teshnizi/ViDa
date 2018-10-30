
#include <chrono>
#include "transmitter.h"


int main(){
    prepare_curl();

    auto start = chrono::system_clock::now();

    //here we calculate an integral:
    cout << endl << integrate("7x/t + 3(t^2)ln(t) over t") << endl << endl;
    auto finish = chrono::system_clock::now();

    chrono::duration<double> diff = finish - start;
    cout << diff.count() << endl << endl << endl;


    string exp1 = "e^(-(x^2))";
    string exp2 = "e^(-(x^2))";

    start = chrono::system_clock::now();

    //here we calculate a convolution:
    cout << convolve(exp1, exp2) << endl << endl;
    finish = chrono::system_clock::now();

    diff = finish - start;
    cout << diff.count() << endl;

    cleanup();
}