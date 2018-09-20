//
// Created by teshnizi on 21/08/18.
//

#ifndef CMAKE_BUILD_DEBUG_CMAKEFILES_VIDAG_TUPLES2_H
#define CMAKE_BUILD_DEBUG_CMAKEFILES_VIDAG_TUPLES2_H

#endif //CMAKE_BUILD_DEBUG_CMAKEFILES_VIDAG_TUPLES2_H


#include <sstream>
#include <string>
#include <algorithm>
#include <vector>
#include <fstream>

using namespace std;

const int discount_coefficient = 100;
const int tax_coefficient = 100;
const int price_coefficient = 10;
const int retailprice_coefficient = 100;

enum Table{
    l_orderkey_id = 0,
    l_quantity_id,
    l_price_id,
    l_discount_id,
    l_shipdate_id,
    l_partkey_id,
    l_suppkey_id,
    l_linenumber_id,
    l_tax_id,
    l_returnflag_id,
    l_linestatus_id,
    l_commitdate_id,
    l_receiptdate_id,
    l_shipinstruct_id,
    l_shipmode_id = 14,
    p_partkey_id = 15,
    p_name_id,
    p_mfgr_id,
    p_brand_id,
    p_type_id,
    p_size_id,
    p_container_id,
    p_retailprice_id,
    p_comment_id = 23,
    ENUM_COUNT
};

const int max_table_num = 2;
const int max_attribute_num = 15; // max number of attributes per table

const int month_days_acc[] = {31, 31+28, 31+28+31, 31+28+31+30, 31+28+31+30+31,
                              31+28+31+30+31+30, 31+28+31+30+31+30+31, 31+28+31+30+31+30+31+31,
                              31+28+31+30+31+30+31+31+30, 31+28+31+30+31+30+31+31+30+31,
                              31+28+31+30+31+30+31+31+30+31+30, 31+28+31+30+31+30+31+31+30+31+30+31};


string days_to_date(int days){
    int year = (days / 365);
    days -= year * 365;
    int month = (lower_bound(month_days_acc,month_days_acc+12, days+1) - month_days_acc);
//    cout << month << endl;
    if ( month > 0 )
        days -= month_days_acc[month-1];
    return to_string(year + 1900) + "-" + to_string(month + 1) + "-" + to_string(days + 1);
}
int date_to_days(string date){
    replace(date.begin(), date.end(), '-', ' ');
    istringstream iss(date);
    int year, month, day, days;
    iss >> year >> month >> day;
    days = (year-1900) * 365;
    (month > 1)?days+=month_days_acc[month-2]:1+1;
    days += day - 1;
    return days;
}

void read_lineitem_from_file(string file_name, vector<int> table_ints[], vector<string> table_strings[], vector<int> int_attributes, vector<int> string_attributes){
    ifstream fin;
    fin.open(file_name);
    string line;
    int int_tmp[ENUM_COUNT];
    float float_tmp[ENUM_COUNT];
    char char_tmp[ENUM_COUNT];
    string string_tmp[ENUM_COUNT];

    while (getline(fin,line,'\n')) {
        istringstream iss(line);
        iss >> int_tmp[l_orderkey_id] >> int_tmp[l_partkey_id] >> int_tmp[l_suppkey_id] >> int_tmp[l_linenumber_id]
            >> int_tmp[l_quantity_id] >> float_tmp[l_price_id] >>
            float_tmp[l_discount_id] >> float_tmp[l_tax_id] >> char_tmp[l_returnflag_id] >> char_tmp[l_linestatus_id]
            >> int_tmp[l_shipdate_id] >>
            int_tmp[l_commitdate_id] >> int_tmp[l_receiptdate_id] >> string_tmp[l_shipinstruct_id] >> string_tmp[l_shipmode_id];

        int_tmp[l_discount_id] = discount_coefficient * float_tmp[l_discount_id];
        int_tmp[l_tax_id] = tax_coefficient * float_tmp[l_tax_id];
        int_tmp[l_price_id] = price_coefficient * float_tmp[l_price_id];
        int_tmp[l_returnflag_id] = char_tmp[l_returnflag_id];
        int_tmp[l_linestatus_id] = char_tmp[l_linestatus_id];

        for (int i = 0; i < int_attributes.size(); ++i)
            table_ints[int_attributes[i]].push_back(int_tmp[int_attributes[i]]);
        for (int i = 0; i < string_attributes.size(); ++i)
            table_strings[string_attributes[i]].push_back(string_tmp[string_attributes[i]]);
    }
}



void read_part_from_file(string file_name, vector<int> table_ints[], vector<string> table_strings[], vector<int> int_attributes, vector<int> string_attributes){
    ifstream fin;
    fin.open(file_name);
    string line;
    int int_tmp[ENUM_COUNT];
    string string_tmp[ENUM_COUNT];
    float float_tmp[ENUM_COUNT];

    while (getline(fin,line,'\n')) {
        istringstream iss(line);
        iss >> int_tmp[p_partkey_id] >> string_tmp[p_name_id] >> string_tmp[p_mfgr_id] >> string_tmp[p_brand_id] >> string_tmp[p_type_id] >> int_tmp[p_size_id] >>
            string_tmp[p_container_id] >> float_tmp[p_retailprice_id] >> string_tmp[p_comment_id];
        int_tmp[p_retailprice_id] = retailprice_coefficient * float_tmp[p_retailprice_id];

        for (int i = 0; i < int_attributes.size(); ++i) {
            table_ints[int_attributes[i]].push_back(int_tmp[int_attributes[i]]);
        }

        for (int i = 0; i < string_attributes.size(); ++i) {
            table_strings[string_attributes[i]].push_back(string_tmp[string_attributes[i]]);
        }

    }
}