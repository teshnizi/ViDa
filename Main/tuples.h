//
// Created by teshnizi on 20/07/18.
//

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

enum lineitem{
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
    l_shipmode_id,
    l_shipinstruct_id,
};

enum part{
    p_partkey_id= 0,
    p_name_id,
    p_mfgr_id,
    p_brand_id,
    p_type_id,
    p_size_id,
    p_container_id,
    p_retailprice_id,
    p_comment_id
};



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

void read_lineitems_from_file(vector<int> *table_ints, vector<string> *table_strings, string name,
int *int_attributes, int num_of_int_attributes, int *string_attributes, int num_of_string_attributes){
    ifstream fin;
    fin.open(name);
    string line;
    int int_tmp[20];
    float float_tmp[20];
    char char_tmp[20];
    string string_tmp[20];

    while (getline(fin,line,'\n')) {
        istringstream iss(line);
        iss >> int_tmp[l_orderkey_id] >> int_tmp[l_partkey_id] >> int_tmp[l_suppkey_id] >> int_tmp[l_linenumber_id] >> int_tmp[l_quantity_id] >> float_tmp[l_price_id] >>
            float_tmp[l_discount_id] >> float_tmp[l_tax_id] >> char_tmp[l_returnflag_id] >> char_tmp[l_linestatus_id] >> int_tmp[l_shipdate_id] >>
            int_tmp[l_commitdate_id] >> int_tmp[l_receiptdate_id] >> string_tmp[l_shipinstruct_id] >> string_tmp[l_shipmode_id];

        int_tmp[l_discount_id] = discount_coefficient * float_tmp[l_discount_id];
        int_tmp[l_tax_id] = tax_coefficient * float_tmp[l_tax_id];
        int_tmp[l_price_id] = price_coefficient * float_tmp[l_price_id];
        int_tmp[l_returnflag_id] = char_tmp[l_returnflag_id];
        int_tmp[l_linestatus_id] = char_tmp[l_linestatus_id];

        for (int i = 0; i < num_of_int_attributes; ++i)
            table_ints[int_attributes[i]].push_back(int_tmp[int_attributes[i]]);
        for (int i = 0; i < num_of_string_attributes; ++i)
            table_strings[string_attributes[i]].push_back(string_tmp[string_attributes[i]]);
    }
}

void read_parts_from_file(vector<int> *table_ints, vector<string> *table_strings, string name,
                          int *int_attributes, int num_of_int_attributes, int *string_attributes, int num_of_string_attributes){
    ifstream fin;
    fin.open(name);
    string line;
    int int_tmp[10];
    string string_tmp[10];
    float float_tmp[10];

    while (getline(fin,line,'\n')) {
        istringstream iss(line);
        iss >> int_tmp[p_partkey_id] >> string_tmp[p_name_id] >> string_tmp[p_mfgr_id] >> string_tmp[p_brand_id] >> string_tmp[p_type_id] >> int_tmp[p_size_id] >>
            string_tmp[p_container_id] >> float_tmp[p_retailprice_id] >> string_tmp[p_comment_id];
        int_tmp[p_retailprice_id] = retailprice_coefficient * float_tmp[p_retailprice_id];

        for (int i = 0; i < num_of_int_attributes; ++i) {
            table_ints[int_attributes[i]].push_back(int_tmp[int_attributes[i]]);
        }
        for (int i = 0; i < num_of_string_attributes; ++i) {
            table_strings[string_attributes[i]].push_back(string_tmp[string_attributes[i]]);
        }

    }
}