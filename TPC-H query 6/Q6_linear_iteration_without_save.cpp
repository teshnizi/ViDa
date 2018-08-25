/*
Created by teshnizi on 31/07/18.
 iterates over original table in order to find acceptable rows
*/

#include <iostream>
#include <fstream>
#include "tuples.h"
#include <math.h>
#include "chrono"

const float eps = 1e-6;
int main(){
    auto start = chrono::system_clock::now();
    chrono::duration<double> diff;

    cout<<"\nReading table from file and checking tuples..."<<endl;

    long ans = 0;
    int start_date = date_to_days("1994-01-01");
    int end_date = date_to_days("1995-01-01");

    ifstream fin;
    fin.open("edited_lineitem.tbl");
    string line;
    int orderkey, quantity, ship_date;
    int discount, extended_price;
    float float_tmp[2];
    int table_size = 0;
    int valid_num = 0;
    int x;
    float y;
    char z;

    int temp[3] = {0,0,0};

    while (getline(fin,line,'\n')) {
        table_size++;
        istringstream iss(line);
        iss >> orderkey >> x >> x >> x >> quantity >> float_tmp[0] >> float_tmp[1] >> y >> z >> z >> ship_date;
        extended_price = float_tmp[0] * 10;
        discount = float_tmp[1] * 100;
        temp[0]++;
        if ( start_date <= ship_date && ship_date < end_date) {
            temp[1]++;
            if (quantity < 24)
                temp[2]++;
            if (5 < discount && discount < 7) {
                ans += extended_price * discount;
                valid_num++;
            }
        }
    }
    for (int i = 0; i < 3; ++i) {
        cout << temp[i] << " " ;
    }cout << valid_num << endl;

    cout<<"Total revenue: " << ans / 1000.0<<endl << "Accepted rows percentage: " << (float)valid_num/table_size * 100.0 <<endl;
    auto end = chrono::system_clock::now();
    diff = end-start;

    cout<<"Done! elapsed time: " << diff.count() <<"s\n\nTable size: " << table_size << endl;

}
