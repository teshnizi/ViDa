//
// Created by teshnizi on 31/07/18.
//


#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>
#include "chrono"
#include "statistics.h"
#include <algorithm>
#include <iomanip>


using namespace std;

const vector <string> valid_strings = {"SM_CASE", "SM_BOX", "SM_PACK", "SM_PKG"};

int main(int argc, char** argv) {
    auto start = chrono::system_clock::now();
    auto end = chrono::system_clock::now();
    chrono::duration<double> diff;
    cout << "\nreading table from file...\n";
    ifstream fin;
    fin.open("edited_part.tbl");
    string line;
    int int_tmp;
    string s;

    vector<string> table(6e6);

    while (getline(fin, line, '\n')) {
        istringstream iss(line);
        iss >> int_tmp >> s >> s >> s >> s >> int_tmp >> s;
        table.push_back(s);
    }

    end = chrono::system_clock::now();
    diff = end - start;
    cout << "Done! elapsed time: " << diff.count() << "s\n\n";

    start = chrono::system_clock::now();
    cout <<"Sorting data(generating histogram)...\n";

    sort(table.begin(), table.end());

    end = chrono::system_clock::now();
    diff = end - start;
    cout << "Done! elapsed time: " << diff.count() << "s\n\n";

    start = chrono::system_clock::now();
    cout <<"Generating data...\n";
    vector <int> upper_indices;
    vector <int> lower_indices;
    vector <int> ranges;
    vector <float> ratios;
    float total_ratio = 0;
    int table_size = table.size();
    vector <int> cnt;

    for (int i = 0; i < valid_strings.size(); ++i) {
        upper_indices.push_back(upper_bound(table.begin(), table.end(), valid_strings[i]) - table.begin());
        lower_indices.push_back(lower_bound(table.begin(), table.end(), valid_strings[i]) - table.begin());
        ranges.push_back(upper_indices[i] - lower_indices[i]);
        ratios.push_back(ranges[i]/(float)table_size);
        total_ratio += ratios[i];
        cnt.push_back(0);
    }
    float output_size = table_size * total_ratio;
    for (int i = 1; i < ranges.size(); ++i) {
        ranges[i] = ranges[i - 1] + ranges[i];
    }

    int k;
    for (int i = 0; i < output_size; ++i) {
        k = rand()%ranges[ranges.size()-1];
        k = lower_bound(ranges.begin(), ranges.end(), k) - ranges.begin();
        cnt[k]++;
    }
    for (int i = 0; i < valid_strings.size(); ++i) {
        cout << cnt[i] << " x " << valid_strings[i] << endl;
    }

    end = chrono::system_clock::now();
    diff = end - start;
    cout << "Done! elapsed time: " << diff.count() << "s\n\n";

}

