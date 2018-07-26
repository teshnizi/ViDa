//
// Created by teshnizi on 20/07/18.
//


//#include "file_functions.h"

#include <chrono>
#include "ND_data_generation.h"
#include <fstream>

using namespace std;


int main(){





    auto start = chrono::system_clock::now();
    auto end = chrono::system_clock::now();
    chrono::duration<double> diff;

    start = chrono::system_clock::now();
    cout<<"reading table from file...\n";

    ifstream fin;
    fin.open("test_data.csv");
    string line;
    vector<line_item> table;
    while (getline(fin,line,'\n')) {
        table.push_back(line_item(line));
    }

    end = chrono::system_clock::now();
    diff = end-start;
    cout<<"Done! elapsed time: " << diff.count() <<"s\n\n";


    int ans = 0;

    //Brute force algorithm. Iterates over the whole table and searches for requested tuples:
    start = chrono::system_clock::now();
    int start_date = string_date_to_days("1994-01-01");
    int end_date =  string_date_to_days("1995-01-01");
    cout<<"Executing brute force algorithm..."<<endl;
    for (int i = 0; i < table.size(); ++i) {
//        if ( i % 1000 == 0 )cout << table[i].to_str()<<endl;
        if ( start_date <= table[i].ship_date && table[i].ship_date < end_date)
            if ( table[i].quantity < 24)
                if ( (0.06 - 0.01) < table[i].discount && table[i].discount < (0.06 + 0.01) )
                    ans += table[i].extended_price * table[i].discount;
    }

    cout<<"Total revenue: " << ans<<endl;
    end = chrono::system_clock::now();
    diff = end-start;
    cout<<"Done! elapsed time: " << diff.count() <<"s\n\n";
}