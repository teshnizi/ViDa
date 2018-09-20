#include <math.h>
#include <stdio.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include "tuples.h"
#include <map>
#include "statistics.h"

using namespace std;


enum Table{
    l_orderkey = 0,
    l_quantity,
    l_price,
    l_discount,
    l_shipdate,
    l_partkey,
    l_suppkey,
    l_linenumber,
    l_tax,
    l_returnflag,
    l_linestatus,
    l_commitdate,
    l_receiptdate,
    l_shipinstruct,
    l_shipmode = 14,
    p_partkey = 15,
    p_name,
    p_mfgr,
    p_brand,
    p_type,
    p_size,
    p_container,
    p_retailprice,
    p_comment = 23,
    ENUM_COUNT
};



map <string, int> mp[ENUM_COUNT];

int main(){

    vector <int> lineitem_table_ints[40];
    vector <int> part_table_ints[40];
    vector <string> part_table_strings[40];
    vector <string> lineitem_table_strings[40];

    ifstream fin;
    fin.open("edited_lineitem.tbl");
    ofstream fout;
    fout.open("histograms.txt");
    string line;
    int int_tmp[40];
    float float_tmp[40];
    char char_tmp[40];
    string string_tmp[40];
    int iterator = 0;

    vector<int> table[ENUM_COUNT];

    while (getline(fin,line,'\n')) {
        istringstream iss(line);
        iss >> int_tmp[l_orderkey] >> int_tmp[l_partkey] >> int_tmp[l_suppkey] >> int_tmp[l_linenumber]
            >> int_tmp[l_quantity] >> float_tmp[l_price] >>
            float_tmp[l_discount] >> float_tmp[l_tax] >> char_tmp[l_returnflag] >> char_tmp[l_linestatus]
            >> int_tmp[l_shipdate] >>
            int_tmp[l_commitdate] >> int_tmp[l_receiptdate] >> string_tmp[l_shipinstruct]
            >> string_tmp[l_shipmode];

        int_tmp[l_discount] = discount_coefficient * float_tmp[l_discount];
        int_tmp[l_tax] = tax_coefficient * float_tmp[l_tax];
        int_tmp[l_price] = price_coefficient * float_tmp[l_price];
        int_tmp[l_returnflag] = char_tmp[l_returnflag];
        int_tmp[l_linestatus] = char_tmp[l_linestatus];

        mp[l_shipmode][string_tmp[l_shipmode]]++;
        mp[l_shipinstruct][string_tmp[l_shipinstruct]]++;
        table[l_discount].push_back(int_tmp[l_discount]);
        table[l_price].push_back(int_tmp[l_price]);
        table[l_partkey].push_back(int_tmp[l_partkey]);
    }
    fin.close();
    fin.open("edited_part.tbl");

    while (getline(fin,line,'\n')) {
        istringstream iss(line);
        iss >> int_tmp[p_partkey] >> string_tmp[p_name] >> string_tmp[p_mfgr] >> string_tmp[p_brand]
            >> string_tmp[p_type] >> int_tmp[p_size] >>
            string_tmp[p_container] >> float_tmp[p_retailprice] >> string_tmp[p_comment];

        int_tmp[p_retailprice] = retailprice_coefficient * float_tmp[p_retailprice];

        table[p_partkey].push_back(int_tmp[p_partkey]);
        table[p_size].push_back(int_tmp[p_size]);
        mp[p_brand][string_tmp[p_brand]]++;
        mp[p_container][string_tmp[p_container]]++;
    }

        fout << 0 << " " << l_shipmode_id << " " << mp[l_shipmode].size() << " " << 0 << " " << 0 << " ";
    for (auto it = mp[l_shipmode].begin(); it != mp[l_shipmode].end(); ++it) {
        cout << it->first << " " << it->second << endl;
        fout << it->second << " ";
    } fout << endl;
    cout << endl;

    fout << 0 << " " << l_shipinstruct_id << " " << mp[l_shipinstruct].size() << " " << 0 << " " << 0 << " ";
    for (auto it = mp[l_shipinstruct].begin(); it != mp[l_shipinstruct].end(); ++it) {
        cout << it->first << " " << it->second << endl;
        fout << it->second << " ";
    } fout << endl;
    cout << endl;

    float block_size[ENUM_COUNT];
    float lower_bounds[ENUM_COUNT];
    vector<int> hist[ENUM_COUNT];

    sort(table[l_discount].begin(), table[l_discount].end());
    hist[l_discount] = quantize(table[l_discount], 11, block_size[l_discount], lower_bounds[l_discount]);
    fout << 0 << " " <<l_discount_id << " " << 11 << " " << lower_bounds[l_discount]
         << " " << lower_bounds[l_discount] + 11 * block_size[l_discount] << " ";
    for (int i = 0; i < 11; ++i) {
        fout << hist[l_discount][i] << " ";
    }fout << endl;
    cout << endl;

    sort(table[l_price].begin(), table[l_price].end());
    hist[l_price] = quantize(table[l_price], 70, block_size[l_price], lower_bounds[l_price]);
    fout << 0 << " " <<l_price_id << " " << 70 << " " << lower_bounds[l_price]
         << " " << lower_bounds[l_price] + 70 * block_size[l_price] << " ";
    for (int i = 0; i < 70; ++i) {
        fout << hist[l_price][i] << " ";
    }fout << endl;
    cout << endl;

    sort(table[l_partkey].begin(), table[l_partkey].end());
    hist[l_partkey] = quantize(table[l_partkey], 70, block_size[l_partkey], lower_bounds[l_partkey]);
    fout << 0 << " " <<l_partkey_id << " " << 70 << " " << lower_bounds[l_partkey]
         << " " << lower_bounds[l_partkey] + 70 * block_size[l_partkey] << " ";
    for (int i = 0; i < 70; ++i) {
        fout << hist[l_partkey][i] << " ";
    }fout << endl;
    cout << endl;


    sort(table[p_partkey].begin(), table[p_partkey].end());
    hist[p_partkey] = quantize(table[p_partkey], 70, block_size[p_partkey], lower_bounds[p_partkey]);
    fout << 1 << " " <<p_partkey_id << " " << 70 << " " << lower_bounds[p_partkey]
         << " " << lower_bounds[p_partkey] + 70 * block_size[p_partkey] << " ";
    for (int i = 0; i < 70; ++i) {
        fout << hist[p_partkey][i] << " ";
    }fout << endl;
    cout << endl;


    fout << 1 << " " << p_brand_id << " " << mp[p_brand].size() << " " << 0 << " " << 0 << " ";
    for (auto it = mp[p_brand].begin(); it != mp[p_brand].end(); ++it) {
        cout << it->first << " " << it->second << endl;
        fout << it->second << " ";
    } fout << endl;
    cout << endl;

    fout << 1 << " " << p_container_id << " " << mp[p_container].size() << " " << 0 << " " << 0 << " ";
    for (auto it = mp[p_container].begin(); it != mp[p_container].end(); ++it) {
        cout << it->first << " " << it->second << endl;
        fout << it->second << " ";
    } fout << endl;
    cout << endl;

    sort(table[p_size].begin(), table[p_size].end());
    hist[p_size] = quantize(table[p_size], 50, block_size[p_size], lower_bounds[p_size]);
    fout << 1 << " " <<p_size_id << " " << 50 << " " << lower_bounds[p_size]
         << " " << lower_bounds[p_size] + 50 * block_size[p_size] << " ";
    for (int i = 0; i < 50; ++i) {
        fout << hist[p_size][i] << " ";
    }fout << endl;
    cout << endl;

    fout.close();
    fin.close();
}