//
// Created by teshnizi on 17/08/18.
//

#include <iostream>
#include <fstream>
#include "tuples.h"
#include <math.h>
#include "chrono"
#include <vector>
#include <unordered_map>

int main(){
    auto start = chrono::system_clock::now();
    auto end = chrono::system_clock::now();
    chrono::duration<double> diff;

    cout<<"\nReading tables from file...\n";

    int lineitem_int_attributes[] = {l_partkey, l_quantity, l_price, l_discount, l_tax};
    int lineitem_string_attributes[] = {l_shipmode, l_shipinstruct};
    int parts_int_attributes[] = {p_partkey, p_size};
    int parts_string_attributes[] = {p_brand, p_container};
    vector <int> lineitem_table_ints[20];
    vector <int> part_table_ints[10];
    vector <string> part_table_strings[10];
    vector <string> lineitem_table_strings[20];

    read_lineitems_from_file(lineitem_table_ints, lineitem_table_strings, "edited_lineitem.tbl",
                             lineitem_int_attributes,5, lineitem_string_attributes, 2);
    read_parts_from_file(part_table_ints, part_table_strings, "edited_part.tbl",parts_int_attributes, 2, parts_string_attributes, 2);

    int lineitem_table_size = lineitem_table_ints[lineitem_int_attributes[0]].size();
    int part_table_size = part_table_ints[parts_int_attributes[0]].size();
    int valid_num = 0;

    end = chrono::system_clock::now();
    diff = end-start;
    cout<<"Done! elapsed time: " << diff.count() <<"s\n\n";

    start = chrono::system_clock::now();
    cout<<"\nPreprocessing(Hashing part table)..."<<endl;

    string container;

    unordered_map <int, int> hash_table;
    for (int i = 0; i < part_table_size; ++i) {
        int partkey = part_table_ints[p_partkey][i];
        if(part_table_ints[p_size][i]>15)
            continue;
        container = part_table_strings[p_container][i];
        if(
                (part_table_strings[p_brand][i] == "Brand#12" && 1 < part_table_ints[p_size][i] && part_table_ints[p_size][i] < 5
                && (container == "SM_CASE" || container == "SM_BOX" || container == "SM_PACK" || container == "SM_PKG")) ||
                (part_table_strings[p_brand][i] == "Brand#23" && 1 < part_table_ints[p_size][i] && part_table_ints[p_size][i] < 10
                && (container == "MED_BAG" || container == "MED_BOX" || container == "MED_PACK" || container == "MED_PKG")) ||
                (part_table_strings[p_brand][i] == "Brand#34" && 1 < part_table_ints[p_size][i] && part_table_ints[p_size][i] < 15
                && (container == "LG_CASE" || container == "LG_BOX" || container == "LG_PACK" || container == "LG_PKG"))
                )
        hash_table[partkey] = i;
    }

    end = chrono::system_clock::now();
    diff = end-start;
    cout<<"Done! elapsed time: " << diff.count() <<"s\n\n";

    start = chrono::system_clock::now();
    cout<<"\nChecking tuples..."<<endl;


    long ans = 0;
    int start_date = date_to_days("1994-01-01");
    int end_date = date_to_days("1995-01-01");
    //    int lsm = 0;
    //    int lqt = 0;
    //    cout << part_table_size << endl;
    //    cout << lineitem_table_size << endl;

    string brand;
//    string container;
    int quantity;
    int size;
    string shipmode;
    string shipinstruct;
    int price;
    int discount;

    for (int i = 0; i < lineitem_table_size; ++i) {
        int p_index = hash_table[lineitem_table_ints[l_partkey][i]];
        if (p_index != part_table_size){
            brand = part_table_strings[p_brand][p_index];
            container = part_table_strings[p_container][p_index];
            quantity = lineitem_table_ints[l_quantity][i];
            size = part_table_ints[p_size][p_index];
            shipmode = lineitem_table_strings[l_shipmode][i];
            shipinstruct = lineitem_table_strings[l_shipinstruct][i];
            price = lineitem_table_ints[l_price][i];
            discount = lineitem_table_ints[l_discount][i];

            if (
                    ((brand == "Brand#12") &&
                (container == "SM_CASE" || container == "SM_BOX" || container == "SM_PACK" || container == "SM_PKG") &&
                (1 <= quantity && quantity <= 11) &&
                (1 < size && size < 5 ) &&
                (shipmode == "AIR" || shipmode == "REG_AIR") &&
                (shipinstruct == "DELIVER_IN_PERSON"))
                    ||
                    ((brand == "Brand#23") &&
                 (container == "MED_BAG" || container == "MED_BOX" || container == "MED_PACK" || container == "MED_PKG") &&
                 (10 <= quantity && quantity <= 20) &&
                 (1 < size && size < 10 ) &&
                 (shipmode == "AIR" || shipmode == "REG_AIR") &&
                 (shipinstruct == "DELIVER_IN_PERSON"))
                    ||
                    ((brand == "Brand#34") &&
                 (container == "LG_CASE" || container == "LG_BOX" || container == "LG_PACK" || container == "LG_PKG") &&
                 (20 <= quantity && quantity <= 30) &&
                 (1 < size && size < 15 ) &&
                 (shipmode == "AIR" || shipmode == "REG_AIR") &&
                 (shipinstruct == "DELIVER_IN_PERSON"))
                    )
            {
//                 cout << price << " " << discount << endl;
//                 cout << brand << " " << quantity << " " << size << " " << container << " " << shipmode << " " << shipinstruct << endl;
                 ans += (price * (discount));
                 valid_num++;
            }
        }
    }

    cout<<"Total revenue: " << ans / 1000.0<<endl << "Accepted rows percentage: " << (float)valid_num/lineitem_table_size * 100.0 <<endl;
    end = chrono::system_clock::now();
    diff = end-start;
    cout<<"Done! elapsed time: " << diff.count() <<"s\n\nTable size: " << lineitem_table_size << endl;
}

