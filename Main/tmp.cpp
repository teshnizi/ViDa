
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <iostream>
#include <chrono>

using namespace std;


string GetStdoutFromCommand(string cmd) {

    string data;
    FILE * stream;
    const int max_buffer = 256;
    char buffer[max_buffer];
    cmd.append(" 2>&1");

    stream = popen(cmd.c_str(), "r");

    if (stream) {
        chrono::high_resolution_clock::time_point st = chrono::high_resolution_clock::now();
        while (!feof(stream) && chrono::duration_cast<chrono::milliseconds>( chrono::high_resolution_clock::now() - st ).count() < 1000) {
            if (fgets(buffer, max_buffer, stream) != NULL) data.append(buffer);
        }
        pclose(stream);
    }
    return data;
}




int main() {

    cout << GetStdoutFromCommand("ssh pi@192.168.43.187 \"wolframscript -format CForm -c 'Expectation[(lPrice * lDiscount), {  lDiscount\\[Distributed]UniformDistribution[{0,10}]}]'\"");

    return 0;
}