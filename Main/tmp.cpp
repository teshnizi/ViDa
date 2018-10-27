
#include <chrono>
#include "transmitter.h"

int main(){
    prepare_curl();
    auto start = chrono::system_clock::now();
    cout << integrate("7x/t + 3ln(xt) over t") << endl;
    auto finish = chrono::system_clock::now();
    chrono::duration<double> diff = finish - start;
    cout << diff.count() << endl;
}