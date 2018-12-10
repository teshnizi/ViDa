/*
Created by teshnizi on 31/07/18.
 iterates over original table in order to find acceptable rows
*/

#include <iostream>
#include <fstream>
#include "tuples.h"
#include <math.h>
#include "chrono"
#include <vector>

int main(){
    auto start = chrono::system_clock::now();
    auto end = chrono::system_clock::now();
    chrono::duration<double> diff;

    cout<<"\nReading table from file...\n";
    ifstream fin;
    fin.open("edited_lineitem.tbl");
    string line;
    int tmp[5];
    float float_tmp[2];
    int table_size = 0;
    int valid_num = 0;
    int x;
    float y;
    char z;
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
    cout<<"\nChecking tuples..."<<endl;

    long ans = 0;
    int start_date = date_to_days("1994-01-01");
    int end_date = date_to_days("1995-01-01");


    for (int i = 0; i < 100; ++i) {
//        if ( start_date <= table[l_shipdate][i] && table[l_shipdate][i] < end_date)
//            if ( table[l_quantity][i] < 24)
//                if ( (5) < table[l_discount][i] && table[l_discount][i] < (7) )
                {
                    cout << table[l_price_id][i] << " " << table[l_discount_id][i] << endl;
                    ans += table[l_price_id][i] * table[l_discount_id][i];
//                    cout << ans << endl;
                    valid_num++;
                }
    }

    cout<<"Total revenue: " << ans / 1000.0<<endl << "Accepted rows percentage: " << (float)valid_num/table_size * 100.0 <<endl;
    end = chrono::system_clock::now();
    diff = end-start;

    cout<<"Done! elapsed time: " << diff.count() <<"s\n\nTable size: " << table_size << endl;

}

