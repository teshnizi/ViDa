//
// Created by teshnizi on 19/08/18.
//

#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>
#include "chrono"
#include "statistics.h"
#include <algorithm>
#include <iomanip>
#include "tuples.h"

using namespace std;


const int num_of_conditions = 3;

vector <string> parts_valid_strings[10][3];
vector <string> lineitem_valid_strings[20][3];

pair<int,int> lineitem_valid_int_ranges[20][3];
pair<int,int> part_valid_int_ranges[10][3];

int main(int argc, char** argv) {

    parts_valid_strings[p_container][0].push_back("SM_CASE");
    parts_valid_strings[p_container][0].push_back("SM_BOX");
    parts_valid_strings[p_container][0].push_back("SM_PACK");
    parts_valid_strings[p_container][0].push_back("SM_PKG");

    parts_valid_strings[p_container][1].push_back("MED_BAG");
    parts_valid_strings[p_container][1].push_back("MED_BOX");
    parts_valid_strings[p_container][1].push_back("MED_PKG");
    parts_valid_strings[p_container][1].push_back("MED_PACK");

    parts_valid_strings[p_container][2].push_back("LG_CASE");
    parts_valid_strings[p_container][2].push_back("LG_BOX");
    parts_valid_strings[p_container][2].push_back("LG_PACK");
    parts_valid_strings[p_container][2].push_back("LG_PKG");

    parts_valid_strings[p_brand][0].push_back("Brand#12");
    parts_valid_strings[p_brand][1].push_back("Brand#23");
    parts_valid_strings[p_brand][2].push_back("Brand#34");

    lineitem_valid_strings[l_shipmode][0].push_back("AIR");
    lineitem_valid_strings[l_shipmode][0].push_back("REG_AIR");

    lineitem_valid_strings[l_shipmode][1].push_back("AIR");
    lineitem_valid_strings[l_shipmode][1].push_back("REG_AIR");

    lineitem_valid_strings[l_shipmode][2].push_back("AIR");
    lineitem_valid_strings[l_shipmode][2].push_back("REG_AIR");

    lineitem_valid_strings[l_shipinstruct][0].push_back("DELIVER_IN_PERSON");
    lineitem_valid_strings[l_shipinstruct][1].push_back("DELIVER_IN_PERSON");
    lineitem_valid_strings[l_shipinstruct][2].push_back("DELIVER_IN_PERSON");
    
    
    lineitem_valid_int_ranges[l_quantity][0].first = 0;
    lineitem_valid_int_ranges[l_quantity][0].second = 11;
    lineitem_valid_int_ranges[l_quantity][1].first = 9;
    lineitem_valid_int_ranges[l_quantity][1].second = 21;
    lineitem_valid_int_ranges[l_quantity][2].first = 19;
    lineitem_valid_int_ranges[l_quantity][2].second = 31;

    part_valid_int_ranges[p_size][0].first = 1;
    part_valid_int_ranges[p_size][0].second = 5;
    part_valid_int_ranges[p_size][1].first = 1;
    part_valid_int_ranges[p_size][1].second = 10;
    part_valid_int_ranges[p_size][2].first = 1;
    part_valid_int_ranges[p_size][2].second = 15;

    auto start = chrono::system_clock::now();
    auto end = chrono::system_clock::now();
    chrono::duration<double> diff;

    cout << "\nreading tables from file...\n";

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

    end = chrono::system_clock::now();
    diff = end - start;
    cout << "Done! elapsed time: " << diff.count() << "s\n\n";


    start = chrono::system_clock::now();
    cout <<"Sorting data(preprocessing)...\n";

    for (int i = 0; i < 5; ++i)
        sort(lineitem_table_ints[lineitem_int_attributes[i]].begin(), lineitem_table_ints[lineitem_int_attributes[i]].end());
    for (int i = 0; i < 2; ++i)
        sort(lineitem_table_strings[lineitem_string_attributes[i]].begin(), lineitem_table_strings[lineitem_string_attributes[i]].end());
    for (int i = 0; i < 2; ++i)
        sort(part_table_ints[parts_int_attributes[i]].begin(), part_table_ints[parts_int_attributes[i]].end());
    for (int i = 0; i < 2; ++i)
        sort(part_table_strings[parts_string_attributes[i]].begin(), part_table_strings[parts_string_attributes[i]].end());

    long price_sum = 0;
    long discount_sum = 0;
    for (int i = 0; i < lineitem_table_size; ++i) {
        price_sum += lineitem_table_ints[l_price][i];
        discount_sum += lineitem_table_ints[l_discount][i];
    }
    float expected_price = price_sum / (float)lineitem_table_size;
    float expected_discount = discount_sum / (float)lineitem_table_size;

    end = chrono::system_clock::now();
    diff = end - start;
    cout << "Done! elapsed time: " << diff.count() << "s\n\n";

    start = chrono::system_clock::now();
    cout <<"Generating data...\n";

    int final_output_size = 0;

    for (int k = 0; k < 3; ++k) {
        int condition_output_size = lineitem_table_size;
        for (int i = 0; i < 2; ++i) {
            float ratio = 0;
            int att = lineitem_string_attributes[i];
            for (int j = 0; j < lineitem_valid_strings[att][k].size(); ++j) {
                string s = lineitem_valid_strings[att][k][j];
                int up = upper_bound(lineitem_table_strings[att].begin(), lineitem_table_strings[att].end(), s) - lineitem_table_strings[att].begin();
                int lo = lower_bound(lineitem_table_strings[att].begin(), lineitem_table_strings[att].end(), s) - lineitem_table_strings[att].begin();
                int range = up - lo;
                ratio += (float)range / lineitem_table_strings[att].size();
            }
            condition_output_size *= ratio;
        }
        for (int i = 0; i < 2; ++i) {
            float ratio = 0;
            int att = parts_string_attributes[i];
            for (int j = 0; j < parts_valid_strings[att][k].size(); ++j) {
                string s = parts_valid_strings[att][k][j];
                int up = upper_bound(part_table_strings[att].begin(), part_table_strings[att].end(), s) - part_table_strings[att].begin();
                int lo = lower_bound(part_table_strings[att].begin(), part_table_strings[att].end(), s) - part_table_strings[att].begin();
                int range = up - lo;
                ratio += (float)range/ part_table_strings[att].size();
            }
            condition_output_size *= ratio;
        }
        for (int i = 0; i < 5; ++i) {
            int att = lineitem_int_attributes[i];
            if(lineitem_valid_int_ranges[att][k].first == lineitem_valid_int_ranges[att][k].second)continue;
            int up = lower_bound(lineitem_table_ints[att].begin(), lineitem_table_ints[att].end(), lineitem_valid_int_ranges[att][k].second) - lineitem_table_ints[att].begin();
            int lo = upper_bound(lineitem_table_ints[att].begin(), lineitem_table_ints[att].end(), lineitem_valid_int_ranges[att][k].first) - lineitem_table_ints[att].begin();
            int range = up - lo;
            float ratio = (float)range/ lineitem_table_ints[att].size();
            condition_output_size *= ratio;
        }
        for (int i = 0; i < 2; ++i) {
            int att = parts_int_attributes[i];
            if(part_valid_int_ranges[att][k].first == part_valid_int_ranges[att][k].second)continue;
            int up = lower_bound(part_table_ints[att].begin(), part_table_ints[att].end(), part_valid_int_ranges[att][k].second) - part_table_ints[att].begin();
            int lo = upper_bound(part_table_ints[att].begin(), part_table_ints[att].end(), part_valid_int_ranges[att][k].first) - part_table_ints[att].begin();
            int range = up - lo;
            float ratio = (float)range/ part_table_ints[att].size();
            condition_output_size *= ratio;
        }
        final_output_size += condition_output_size;
    }

    cout <<"Acceptance percentage:" << (float)final_output_size / lineitem_table_size * 100 << endl;
    cout << "Revenue = "
            << final_output_size * (expected_price) * (100 - expected_discount)
               /(discount_coefficient * price_coefficient)
            << endl;
    end = chrono::system_clock::now();
    diff = end-start;
    cout<<"Done! elapsed time: " << diff.count() <<"s\n\n";
}