//
// Created by teshnizi on 31/07/18.
//


#include <iostream>
#include <fstream>
#include "tuples.h"
#include <cmath>
#include "chrono"
#include <vector>
#include "statistics.h"
#include <algorithm>
#include <iomanip>


using namespace std;

const int segment_count = 10;

float query_lower_bounds[5] = {0, 0, 0, 0.06 - 0.01, date_to_days("1994-01-01")};
float query_upper_bounds[5] = {0, 24, 1e9, 0.06 + 0.01, date_to_days("1995-01-01")};

int main(int argc, char** argv)
{
    auto start = chrono::system_clock::now();
    auto end = chrono::system_clock::now();
    chrono::duration<double> diff;
    cout<<"\nreading table from file...\n";
    ifstream fin;
    fin.open("edited_lineitem.tbl");
    string line;
    float tmp[5];
    int table_size = 0;

    vector <float> table[5];

    while (getline(fin,line,'\n')) {
        table_size++;
        istringstream iss(line);
        iss >> tmp[0];
        for (int i = 1; i < 5; ++i) {
            iss >> tmp[i];
            table[i].push_back(tmp[i]);
        }
    }

    end = chrono::system_clock::now();
    diff = end-start;
    cout<<"Done! elapsed time: " << diff.count() <<"s\n\n";

    start = chrono::system_clock::now();
    cout<<"Finding histograms...\n";


    for (int i = 1; i < 5; ++i)
        sort(table[i].begin(), table[i].end());

    float ratios[5];
    int lower_indices[5];
    int upper_indices[5];
    int ranges[5];

    for (int i = 1; i < 5; ++i) {
        upper_indices[i] = upper_bound(table[i].begin(), table[i].end(), query_upper_bounds[i]) - table[i].begin();
        lower_indices[i] = lower_bound(table[i].begin(), table[i].end(), query_lower_bounds[i]) - table[i].begin();
        ranges[i] = upper_indices[i] - lower_indices[i];
        ratios[i] = (float)ranges[i]/(float)table_size;
    }

    float output_size = table_size;
    for (int i = 1; i < 5; ++i)
        output_size *= ratios[i];

    end = chrono::system_clock::now();
    diff = end-start;
    cout<<"Done! elapsed time: " << diff.count() <<"s\n\n";



    start = chrono::system_clock::now();
    srand(time(NULL));
    cout << "Generating random data...\n";
    int ans = 0;
    for (int i = 0; i < output_size; ++i) {
        for (int j = 1; j < 5; ++j) {
            int x = (rand() % ranges[j]) + lower_indices[j];
            tmp[j] = table[j][x];
        }
        ans += tmp[price] * tmp[discount];
    }

    cout<<"Total revenue: " << ans<<endl;
    end = chrono::system_clock::now();
    diff = end-start;
    cout<<"Done! elapsed time: " << diff.count() <<"s\n\nOriginal table size: " << table_size << endl;
}

