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

const int num_of_attributes = ENUM_COUNT;


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
        iss >> int_tmp[l_orderkey] >> int_tmp[l_partkey] >> int_tmp[l_suppkey] >> int_tmp[l_linenumber]
            >> int_tmp[l_quantity] >> float_tmp[l_price] >>
            float_tmp[l_discount] >> float_tmp[l_tax] >> char_tmp[l_returnflag] >> char_tmp[l_linestatus]
            >> int_tmp[l_shipdate] >>
            int_tmp[l_commitdate] >> int_tmp[l_receiptdate] >> string_tmp[l_shipinstruct] >> string_tmp[l_shipmode];

        int_tmp[l_discount] = discount_coefficient * float_tmp[l_discount];
        int_tmp[l_tax] = tax_coefficient * float_tmp[l_tax];
        int_tmp[l_price] = price_coefficient * float_tmp[l_price];
        int_tmp[l_returnflag] = char_tmp[l_returnflag];
        int_tmp[l_linestatus] = char_tmp[l_linestatus];

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
        iss >> int_tmp[p_partkey] >> string_tmp[p_name] >> string_tmp[p_mfgr] >> string_tmp[p_brand] >> string_tmp[p_type] >> int_tmp[p_size] >>
            string_tmp[p_container] >> float_tmp[p_retailprice] >> string_tmp[p_comment];
        int_tmp[p_retailprice] = retailprice_coefficient * float_tmp[p_retailprice];

        for (int i = 0; i < int_attributes.size(); ++i) {
            table_ints[int_attributes[i]].push_back(int_tmp[int_attributes[i]]);
        }

        for (int i = 0; i < string_attributes.size(); ++i) {
            table_strings[string_attributes[i]].push_back(string_tmp[string_attributes[i]]);
        }

    }
}