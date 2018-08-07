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

    float min_value[5], max_value[5];
    for (int i = 0; i < 5; ++i) {
        min_value[i] = 1e9;
        max_value[i] = 0;
    }

    cout<<"\nreading table from file in order to find min and max...\n";
    ifstream fin;
    fin.open("edited_lineitem.tbl");
    string line;
    float tmp[5];
    int table_size = 0;

    while (getline(fin,line,'\n')) {
        table_size++;
        istringstream iss(line);

        for (int i = 0; i < 5; ++i) {
            iss >> tmp[i];
        }
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

    float range[5];
    for (int i = 0; i < 5; ++i)
        range[i] = max_value[i] - min_value[i];

    int start_date = date_to_days("1994-01-01");
    int end_date = date_to_days("1995-01-01");
    long ans = 0;

    for (int i = 0; i < table_size; ++i){
        float textended_price = (rand()/(float)RAND_MAX) * range[price] + min_value[price];
        float tdiscount = (rand()/(float)RAND_MAX) * range[discount] + min_value[discount];
        float tship_date = (rand()/(float)RAND_MAX) * range[ship_date] + min_value[ship_date];
        float tquantity = (rand()/(float)RAND_MAX) * range[quantity] + min_value[quantity];


        if ( start_date <= tship_date && tship_date < end_date)
            if ( tquantity < 24)
                if ( (0.06 - 0.01) < tdiscount && tdiscount < (0.06 + 0.01) )
                    ans += textended_price * tdiscount;
    }

    cout<<"Total revenue: " << ans<<endl;
    end = chrono::system_clock::now();
    diff = end-start;
    cout<<"Done! elapsed time: " << diff.count() <<"s\n\nOriginal table size: " << table_size << endl;

}
