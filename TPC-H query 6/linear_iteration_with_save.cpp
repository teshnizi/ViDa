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
    cout<<"\nChecking tuples..."<<endl;

    long ans = 0;
    int start_date = date_to_days("1994-01-01");
    int end_date = date_to_days("1995-01-01");

    for (int i = 0; i < table_size; ++i) {
        if ( start_date <= table[ship_date][i] && table[ship_date][i] < end_date)
            if ( table[quantity][i] < 24)
                if ( (0.06 - 0.01) < table[discount][i] && table[discount][i] < (0.06 + 0.01) )
                    ans += table[price][i] * table[discount][i];
    }

    cout<<"Total revenue: " << ans<<endl;
    end = chrono::system_clock::now();
    diff = end-start;

    cout<<"Done! elapsed time: " << diff.count() <<"s\n\nTable size: " << table_size << endl;

}

