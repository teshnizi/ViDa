/*
Created by teshnizi on 31/07/18.
finds maximum and minimum of data and starts generating uniformly distributed random numbers and selecting those
which satisfy query conditions.
*/

#include <iostream>
#include <fstream>
#include "tuples.h"
#include <math.h>
#include "chrono"

int main(){
    auto start = chrono::system_clock::now();
    auto end = chrono::system_clock::now();
    chrono::duration<double> diff;

    int min_value[5], max_value[5];
    for (int i = 0; i < 5; ++i) {
        min_value[i] = 1e9;
        max_value[i] = 0;
    }

    cout<<"\nreading table from file in order to find min and max...\n";
    ifstream fin;
    fin.open("edited_lineitem.tbl");
    string line;
    int tmp[5];
    float float_tmp[2];
    int table_size = 0;

    while (getline(fin,line,'\n')) {
        table_size++;
        istringstream iss(line);

        iss >> tmp[0] >> tmp[1] >> float_tmp[0] >> float_tmp[1] >> tmp[4];
        tmp[2] = float_tmp[0] * 10;
        tmp[3] = float_tmp[1] * 100;
        for (int i = 1; i < 5; ++i) {
            min_value[i] = min(min_value[i], tmp[i]);
            max_value[i] = max(max_value[i], tmp[i]);
        }
    }

    end = chrono::system_clock::now();
    diff = end-start;
    cout<<"Done! elapsed time: " << diff.count() <<"s\n\n";

    start = chrono::system_clock::now();
    srand(time(NULL));
    cout << "Generating random data...\n";

    int range[5];
    for (int i = 0; i < 5; ++i)
        range[i] = max_value[i] - min_value[i];

    int start_date = date_to_days("1994-01-01");
    int end_date = date_to_days("1995-01-01");
    long ans = 0;
    int valid_num =0;

    for (int i = 0; i < table_size; ++i){

        int tship_date = (rand()/(float)RAND_MAX) * range[ship_date] + min_value[ship_date];


        if ( start_date <= tship_date && tship_date < end_date) {
            int tquantity = (rand() / (float) RAND_MAX) * range[quantity] + min_value[quantity];
            if (tquantity < 24) {
                int tdiscount = (rand()/(float)RAND_MAX) * range[discount] + min_value[discount];
                if (5 < tdiscount && tdiscount < 7) {
                    int textended_price = (rand()/(float)RAND_MAX) * range[price] + min_value[price];
                    ans += textended_price * tdiscount;
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
