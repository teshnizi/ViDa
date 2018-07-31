//
// Created by teshnizi on 20/07/18.
//


//#include "file_functions.h"

#include <chrono>
#include "ND_data_generation.h"
#include <fstream>

using namespace std;

int main(){

    ofstream time_log("time_log.txt");
    auto start = chrono::system_clock::now();
    auto end = chrono::system_clock::now();
    chrono::duration<double> diff;


    //executing query based on virtual data:

    start = chrono::system_clock::now();
    vector <lineitem> v = execute_query(date_to_days("1994-01-01"), date_to_days("1995-01-01"),0,1000,0.06-0.01, 0.06+0.01 ,0,24);
    cout << "generating virtual data..." << endl;
    ofstream gen_data_file("generated_data.csv");
    for (int i = 0; i < v.size(); ++i) {

        gen_data_file << v[i].key << "," << v[i].ship_date << "," <<
                      v[i].extended_price << "," << v[i].discount << "," << v[i].quantity << endl;
    }
    gen_data_file.close();

    end = chrono::system_clock::now();
    diff = end-start;
    cout<<v.size() << " tuples generated\n";
    cout<<"Done! elapsed time: " << diff.count() <<"s\n\n";
    time_log << "Data generation based on distributions: " << diff.count() <<"\n\n";



// executing query on a real table:

    start = chrono::system_clock::now();
    cout<<"\nreading table from file...\n";
    ifstream fin;
    fin.open("edited_lineitem.tbl");
    string line;
    vector<lineitem> table;
    while (getline(fin,line,'\n')) {
        table.push_back(create_lineitem_from_original_tuple(line));
    }

    end = chrono::system_clock::now();
    diff = end-start;
    cout<<"Done! elapsed time: " << diff.count() <<"s\n\n";



    //Brute force algorithm. Iterates over the whole table and searches for requested tuples:
    //----------------------------------------------------------------------------------------------------------
    start = chrono::system_clock::now();
    int ans = 0;
    int start_date = date_to_days("1994-01-01");
    int end_date = date_to_days("1995-01-01");
    cout<<"Executing the brute force algorithm..."<<endl;
    for (int i = 0; i < table.size(); ++i) {
        if ( start_date <= table[i].ship_date && table[i].ship_date < end_date)
            if ( table[i].quantity < 24)
                if ( (0.06 - 0.01) < table[i].discount && table[i].discount < (0.06 + 0.01) )
                    ans += table[i].extended_price * table[i].discount;
    }
    cout<<"Total revenue: " << ans<<endl;
    end = chrono::system_clock::now();
    diff = end-start;
    cout<<"Done! elapsed time: " << diff.count() <<"s\n\n";
    //----------------------------------------------------------------------------------------------------------


}