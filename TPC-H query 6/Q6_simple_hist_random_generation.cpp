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


const int segment_count = 32000;

int main(){
    auto start = chrono::system_clock::now();
    auto end = chrono::system_clock::now();
    chrono::duration<double> diff;

    cout<<"\nreading table from file...\n";
    ifstream fin;
    fin.open("edited_lineitem.tbl");
    string line;
    int tmp[5], x;
    int table_size = 0;
    char z;
    float float_tmp[2], y;

    vector <int> table[5];

    while (getline(fin,line,'\n')) {
        table_size++;
        istringstream iss(line);
        iss >> tmp[0] >> x >> x >> x >> tmp[1] >> float_tmp[0] >> float_tmp[1] >> y >> z >> z >> tmp[4];
        tmp[2] = 10 * float_tmp[0];
        tmp[3] = 100 * float_tmp[1];
        for (int i = 1; i < 5; ++i) {
            table[i].push_back(tmp[i]);
        }
    }

    end = chrono::system_clock::now();
    diff = end-start;
    cout<<"Done! elapsed time: " << diff.count() <<"s\n\n";


    start = chrono::system_clock::now();
    cout<<"Finding histograms...\n";

    int block_size[5];
    int lower_bounds[5];
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

    auto ss = chrono::system_clock::now();
    auto ee = chrono::system_clock::now();

    int start_date = date_to_days("1994-01-01");
    int end_date = date_to_days("1995-01-01");
    long ans = 0;
    int k;
    int valid_num =0;
    int hist_sizes[5];

    for (int m = 1; m < 5; ++m) {
        hist_sizes[m] = hist[m][hist[m].size() - 1];
    }
    for (int i = 0; i < table_size; ++i){
        k = rand() % hist_sizes[l_shipdate];
        k = lower_bound(hist[l_shipdate].begin(), hist[l_shipdate].end(), k) - hist[l_shipdate].begin();
        tmp[l_shipdate] = ((((float)rand() / RAND_MAX) + k ) * block_size[l_shipdate] + lower_bounds[l_shipdate]);
        if ( start_date <= tmp[l_shipdate] && tmp[l_shipdate] < end_date) {
            k = rand() % hist_sizes[l_quantity];
            k = lower_bound(hist[l_quantity].begin(), hist[l_quantity].end(), k) - hist[l_quantity].begin();
            tmp[l_quantity] = ((((float)rand() / RAND_MAX) + k ) * block_size[l_quantity] + lower_bounds[l_quantity]);
            if (tmp[l_quantity] < 24) {
                k = rand() % hist_sizes[l_discount];
                k = lower_bound(hist[l_discount].begin(), hist[l_discount].end(), k) - hist[l_discount].begin();
                tmp[l_discount] = ((((float)rand() / RAND_MAX) + k ) * block_size[l_discount] + lower_bounds[l_discount]);
                if (5 < tmp[l_discount] && tmp[l_discount] < 7) {
                    k = rand() % hist_sizes[l_price];
                    k = lower_bound(hist[l_price].begin(), hist[l_price].end(), k) - hist[l_price].begin();
                    tmp[l_price] = ((((float)rand() / RAND_MAX) + k ) * block_size[l_price] + lower_bounds[l_price]);
                    ans += tmp[l_price] * tmp[l_discount];
                    valid_num++;
                }
            }
        }
    }

    cout<<"Total revenue: " << ans / 1000.0<<endl << "Accepted rows percentage: " << (float)valid_num/table_size * 100.0 <<endl;
    end = chrono::system_clock::now();
    diff = end-start;
    cout<<"Done! elapsed time: " << diff.count() <<"s\n\nOriginal table size: " << table_size << endl;
}