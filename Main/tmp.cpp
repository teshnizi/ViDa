//
// Created by teshnizi on 15/11/18.
//

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <chrono>
#include <sstream>

using namespace std;

auto start = chrono::system_clock::now();
auto finish = chrono::system_clock::now();
chrono::duration<double> diff = finish - start;

FILE *Maxima;
FILE *Sage;

char buffer[1024];
string result = "";

int startMaxima(){
    Maxima = popen("maxima", "w");
    if (!Maxima)
    {
        fprintf (stderr,
                 "incorrect parameters or too many files.\n");
        return EXIT_FAILURE;
    }
}

int startSage(){
    Sage = popen("sage", "w");
    if (!Sage)
    {
        fprintf (stderr,
                 "incorrect parameters or too many files.\n");
        return EXIT_FAILURE;
    }
}

void CASExecute (FILE * stream, const char* command) {
//    ofstream of(stream);
    fprintf (stream, command);
    if (ferror (stream))
    {
        fprintf (stderr, "Output to stream failed.\n");
        exit (EXIT_FAILURE);
    }
}

chrono::duration<double> MaximaST;
chrono::duration<double> MaximaET;

int main (void)
{

    start = chrono::system_clock::now();
    startMaxima();
    CASExecute(Maxima, "display2d:false;\n");
    CASExecute(Maxima, "load(\"distrib\");\n");

    MaximaST = chrono::system_clock::now() - start;

    start = chrono::system_clock::now();
//    integrate("7/t + 3(t^2)ln(t) over t from 1 to 10")
    CASExecute (Maxima, "f(x) := pdf_normal(x,0,3);\n");
    CASExecute (Maxima, "g(x) := pdf_normal(x,0,4);\n");
    CASExecute (Maxima, "fCONVg(x) := integrate(f(t) * g(x-t), t, -inf, inf);\n");
    CASExecute (Maxima, "fCONVg(x);\n");

//    CASExecute (Maxima, "integrate(f(x), x, -inf, inf);\n");
    MaximaST = chrono::system_clock::now() - start;

    if (pclose (Maxima) != 0) {
        fprintf (stderr,
                 "Could not run more or other error.\n");
    }

    cout << "\n\nMaxima Starting Time: " << MaximaST.count();
    cout << "\nMaxima Execution Time: " << MaximaET.count() << "\n\n";
    cout << endl << endl << endl;

//    startSage();
//    CASExecute(Sage, "1 + 2\n");
//    if (pclose (Sage) != 0) {
//        fprintf (stderr,
//                 "Could not run more or other error.\n");
//    }
//    return EXIT_SUCCESS;
}