/*
Created by teshnizi on 31/07/18.
 iterates over original table in order to find acceptable rows
*/



#include <iostream>
#include <fstream>
#include "tuples.h"
#include <math.h>
#include "chrono"

int main(){

    auto start = chrono::system_clock::now();

    cout<<"\nReading table from file and checking tuples..."<<endl;

    chrono::duration<double> diff;

    int ans = 0;
    int start_date = date_to_days("1994-01-01");
    int end_date = date_to_days("1995-01-01");

    ifstream fin;
    fin.open("edited_lineitem.tbl");
    string line;
    int orderkey, quantity, ship_date;
    float discount, extendedprice;
    int table_size = 0;

    while (getline(fin,line,'\n')) {
        table_size++;
        istringstream iss(line);
        iss >> orderkey >> quantity >> extendedprice >> discount >> ship_date;
        if ( start_date <= ship_date && ship_date < end_date)
            if ( quantity < 24)
                if ( (0.06 - 0.01) < discount && discount < (0.06 + 0.01) )
                    ans += extendedprice * discount;
    }

    cout<<"Total revenue: " << ans<<endl;
    auto end = chrono::system_clock::now();
    diff = end-start;

    cout<<"Done! elapsed time: " << diff.count() <<"s\n\nTable size: " << table_size << endl;

}

