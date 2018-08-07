//
// Created by teshnizi on 31/07/18.
// Generates data based on original data approximate distributions
//

#include <iostream>
#include <fstream>
#include "tuples.h"
#include <math.h>
#include "chrono"
#include <vector>
#include "statistics.h"
#include <algorithm>


const int segment_count = 30;


int main(){
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

    float block_size[5];
    float lower_bounds[5];
    vector<int> hist[5];

    for (int i = 1; i < 5; ++i) {
        sort(table[i].begin(), table[i].end());
        hist[i] = quantize(table[i], segment_count, block_size[i], lower_bounds[i]);
    }

    //converting histograms to cumulative histograms:
    for (int j = 1; j < 5; ++j) {
        for (int i = 1; i < segment_count; ++i) {
            hist[j][i] += hist[j][i-1];
        }
    }

    end = chrono::system_clock::now();
    diff = end-start;
    cout<<"Done! elapsed time: " << diff.count() <<"s\n\n";


    start = chrono::system_clock::now();
    srand(time(NULL));
    cout << "Generating random data...\n";

    int start_date = date_to_days("1994-01-01");
    int end_date = date_to_days("1995-01-01");
    long ans = 0;

    for (int i = 0; i < table_size; ++i){

        for (int j = 1; j < 5; ++j)
            tmp[j] = get_random_data_by_hist(hist[j], block_size[j], lower_bounds[j]);

        if ( start_date <= tmp[ship_date] && tmp[ship_date] < end_date)
            if ( tmp[quantity] < 24)
                if ( (0.06 - 0.01) < tmp[discount] && tmp[discount] < (0.06 + 0.01) )
                    ans += tmp[price] * tmp[discount];
    }

    cout<<"Total revenue: " << ans<<endl;
    end = chrono::system_clock::now();
    diff = end-start;
    cout<<"Done! elapsed time: " << diff.count() <<"s\n\nOriginal table size: " << table_size << endl;
}