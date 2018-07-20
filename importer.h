//
// Created by teshnizi on 20/07/18.
//


#include <iostream>
#include <fstream>
#include <set>
#include <string>
#include <algorithm>
#include <sstream>
#include <utility>

using namespace std;

void import_from_csv(){

    ifstream fin;
    fin.open("test_data.csv");
    ofstream  key_table, date_table;
    key_table.open("database_files/key_indices.txt");
    date_table.open("database_files/date_indices.txt");

    int tmp_index;
    int tmp_extended_price;
    float tmp_discount;
    int tmp_quantity;

    string line;

    set<pair<int, int>> key_indices;
    set<pair<string, int>> date_indices;


    int counter = 0;

    while (getline(fin,line,'\n')){
        replace(line.begin(), line.end(), ',', ' ');
        istringstream iss(line);
        string date_string;
        iss >> tmp_index >> tmp_extended_price >> tmp_discount >> date_string >> tmp_quantity;

        pair<int, int> key_pair = make_pair(tmp_index, counter);
        key_indices.insert(key_pair);


        pair<string, int> date_pair = make_pair(date_string, counter++);
        date_indices.insert(date_pair);

    }

    set<pair<int, int>>::iterator key_it = key_indices.begin();
    set<pair<string, int>>::iterator date_it = date_indices.begin();

    for(;key_it != key_indices.end(); key_it++){
        key_table << (*key_it).first << " " <<(*key_it).second<<endl;
    }

    for(;date_it != date_indices.end(); date_it++){
        date_table << (*date_it).first << " " <<(*date_it).second<<endl;
    }

    key_table.close();
    date_table.close();
    fin.close();
}