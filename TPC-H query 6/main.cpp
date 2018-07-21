//
// Created by teshnizi on 20/07/18.
//

#include <iostream>
#include "file_functions.h"
#include <vector>
#include <chrono>


using namespace std;


int main(){

    cout<<endl;
    auto start = chrono::system_clock::now();
    auto end = chrono::system_clock::now();
    chrono::duration<double> diff;




//    start = chrono::system_clock::now();
//    cout<<"\nReading CSV file and creating indices on \"key\" and \"date\"...\n";
//    create_indices_from_csv();
//    end = chrono::system_clock::now();
//    diff = end-start;
//    cout<<"Done! elapsed time: " << diff.count() <<"s\n\n";
//
//
//    start = chrono::system_clock::now();
//    cout<<"reading indices from file and creating sets...\n";
//    set<pair<string, int>> date_indices = read_string_indices_from_file("database_files/date_indices.txt");
//    set<pair<int, int>> key_indices = read_int_indices_from_file("database_files/key_indices.txt");
//    end = chrono::system_clock::now();
//    diff = end-start;
//    cout<<"Done! elapsed time: " << diff.count() <<"s\n\n";





    start = chrono::system_clock::now();
    cout<<"reading table from file...\n";
    vector<line_item> table = read_table_from_file("test_data.csv");
    end = chrono::system_clock::now();
    diff = end-start;
    cout<<"Done! elapsed time: " << diff.count() <<"s\n\n";


    int ans = 0;

    //Brute force algorithm. Iterates over the whole table and searches for requested tuples:
    start = chrono::system_clock::now();
    cout<<"Executing brute force algorithm..."<<endl;
    for (int i = 0; i < table.size(); ++i) {
        if ( "1994-01-01" <= table[i].ship_date && table[i].ship_date < "1995-01-01")
            if ( table[i].quantity < 24)
                if ( (0.06 - 0.01) < table[i].discount && table[i].discount < (0.06 + 0.01) )
                    ans += table[i].extended_price * table[i].discount;
    }
    cout<<"Total revenue: " << ans<<endl;
    end = chrono::system_clock::now();
    diff = end-start;
    cout<<"Done! elapsed time: " << diff.count() <<"s\n\n";






//    start = chrono::system_clock::now();
//    cout<<"Executing query using indices...\n";
//    set<pair<string, int>>::iterator it = date_indices.lower_bound(make_pair("1994-01-01",0));
//    set<pair<string, int>>::iterator ed = date_indices.lower_bound(make_pair("1995-01-01",0));
//
//    pair<string, int> p;
//
//
//    while (it != ed){
//        if( (0.06 - 0.01) < table[(*it).second].discount && table[(*it).second].discount < (0.06 + 0.01))
//            if( table[(*it).second].quantity < 24) {
////                cout << table[(*it).second].to_str() << endl;
////                cout <<"revenue: " << table[(*it).second].extended_price * table[(*it).second].discount << endl;
//                ans += table[(*it).second].extended_price * table[(*it).second].discount;
//            }
//        it++;
//    }
//    cout<<"Total revenue: " << ans<<endl;
//    end = chrono::system_clock::now();
//    diff = end-start;
//    cout<<"Done! elapsed time: " << diff.count() <<"s\n\n";

}

