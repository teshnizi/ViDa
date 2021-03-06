//
// Created by teshnizi on 21/08/18.
//

#include "tuples2.h"
#include <map>
#include <iostream>
#include <chrono>
#include "generate_from_distribution.h"

const int max_attribute_count = 30;
const int num_of_conditions = 3;

vector<int> table_ints[max_attribute_count];
vector<string> table_strings[max_attribute_count];

map<string, int> string_frequencies[max_attribute_count];

vector <string> valid_strings[max_attribute_count][num_of_conditions];
pair<int,int> valid_int_ranges[max_attribute_count][num_of_conditions];

vector<int> lineitem_ints;
vector<int> lineitem_strings;
vector<int> part_ints;
vector<int> part_strings;

vector <double> params[max_attribute_count];

int num_of_real_table_acceptances[num_of_conditions];

int lineitem_table_size;
int part_table_size;

long summations[max_attribute_count];

//keeps the probability of that an entry inside the virtual table satisfies conditions of the query:
float dist_table_probs[num_of_conditions];

void set_query(){

    lineitem_ints.push_back        (l_partkey_id);
    lineitem_ints.push_back        (l_quantity_id);
    lineitem_ints.push_back        (l_price_id);
    lineitem_ints.push_back        (l_discount_id);
    lineitem_ints.push_back        (l_tax_id);
    lineitem_strings.push_back     (l_shipmode_id);
    lineitem_strings.push_back     (l_shipinstruct_id);
    part_ints.push_back            (p_partkey_id);
    part_ints.push_back            (p_size_id);
    part_strings.push_back        (p_brand_id);
    part_strings.push_back        (p_container_id);

    valid_strings[p_container_id][0].push_back("SM_CASE");
    valid_strings[p_container_id][0].push_back("SM_BOX");
    valid_strings[p_container_id][0].push_back("SM_PACK");
    valid_strings[p_container_id][0].push_back("SM_PKG");
    valid_strings[p_container_id][1].push_back("MED_BAG");
    valid_strings[p_container_id][1].push_back("MED_BOX");
    valid_strings[p_container_id][1].push_back("MED_PKG");
    valid_strings[p_container_id][1].push_back("MED_PACK");
    valid_strings[p_container_id][2].push_back("LG_CASE");
    valid_strings[p_container_id][2].push_back("LG_BOX");
    valid_strings[p_container_id][2].push_back("LG_PACK");
    valid_strings[p_container_id][2].push_back("LG_PKG");
    valid_strings[p_brand_id][0].push_back("Brand#12");
    valid_strings[p_brand_id][1].push_back("Brand#23");
    valid_strings[p_brand_id][2].push_back("Brand#34");
    valid_strings[l_shipmode_id][0].push_back("AIR");
    valid_strings[l_shipmode_id][0].push_back("REG_AIR");
    valid_strings[l_shipmode_id][1].push_back("AIR");
    valid_strings[l_shipmode_id][1].push_back("REG_AIR");
    valid_strings[l_shipmode_id][2].push_back("AIR");
    valid_strings[l_shipmode_id][2].push_back("REG_AIR");
    valid_strings[l_shipinstruct_id][0].push_back("DELIVER_IN_PERSON");
    valid_strings[l_shipinstruct_id][1].push_back("DELIVER_IN_PERSON");
    valid_strings[l_shipinstruct_id][2].push_back("DELIVER_IN_PERSON");


    valid_int_ranges[l_quantity_id][0].first = 0;
    valid_int_ranges[l_quantity_id][0].second = 11;
    valid_int_ranges[l_quantity_id][1].first = 9;
    valid_int_ranges[l_quantity_id][1].second = 21;
    valid_int_ranges[l_quantity_id][2].first = 19;
    valid_int_ranges[l_quantity_id][2].second = 31;
    valid_int_ranges[p_size_id][0].first = 1;
    valid_int_ranges[p_size_id][0].second = 5;
    valid_int_ranges[p_size_id][1].first = 1;
    valid_int_ranges[p_size_id][1].second = 10;
    valid_int_ranges[p_size_id][2].first = 1;
    valid_int_ranges[p_size_id][2].second = 15;

    for (int i = 0; i < num_of_conditions; ++i) {
        dist_table_probs[i] = 1;
    }

    for (int i = 0; i < max_attribute_count; ++i) {
        params[i].push_back(10); //mean
        params[i].push_back(25); //variance
    }
}

void strings_preprocess(){
    for (int i = 0; i < part_table_size; ++i) {
        for (int j = 0; j < part_strings.size(); ++j) {
            string_frequencies[part_strings[j]][table_strings[part_strings[j]][i]]++;
        }
    }
    for (int i = 0; i < num_of_conditions; ++i) {
        for (int j = 0; j < part_strings.size(); ++j) {
            int att = part_strings[j];
            if (valid_strings[att][i].size() == 0)continue;
            int total_valids = 0;
            for (int k = 0; k < valid_strings[att][i].size(); ++k) {
                string s = valid_strings[att][i][k];
                total_valids += string_frequencies[att][s];
            }
            dist_table_probs[i] *= (float) total_valids / table_strings[att].size();
        }
    }
}

void preprocess_using_real_table(){

    for (int i = 0; i < max_attribute_count; ++i) {
        sort(table_ints[i].begin(), table_ints[i].end());
        for (int j = 0; j < table_ints[i].size(); ++j) {
            summations[i] += table_ints[i][j];
        }
    }

    strings_preprocess();

    for (int i = 0; i < num_of_conditions; ++i) {
        for (int j = 0; j < part_ints.size(); ++j) {
            int att = part_ints[j];
            if (valid_int_ranges[att][i].first == valid_int_ranges[att][i].second)continue;
            int up = lower_bound(table_ints[att].begin(), table_ints[att].end(), valid_int_ranges[att][i].second) - table_ints[att].begin();
            int lo = upper_bound(table_ints[att].begin(), table_ints[att].end(), valid_int_ranges[att][i].first)  - table_ints[att].begin();
            dist_table_probs[i] *= float(up - lo)/table_ints[att].size();
        }
    }
}

void preprocess_using_distribution_parameters(){
    strings_preprocess();
    for (int i = 0; i < num_of_conditions; ++i) {
        for (int j = 0; j < part_ints.size(); ++j) {
            int att = part_ints[j];
            if (valid_int_ranges[att][i].first == valid_int_ranges[att][i].second)continue;
            dist_table_probs[i] *= normal_probability(
                    params[att][0], params[att][1], valid_int_ranges[att][i].first, valid_int_ranges[att][i].second);
        }
    }
}

int main(){

    auto start = chrono::system_clock::now();
    auto end = chrono::system_clock::now();
    chrono::duration<double> diff;

    cout<<"\nReading tables from file...\n";
    set_query();
    read_lineitem_from_file("edited_lineitem.tbl", table_ints, table_strings, lineitem_ints, lineitem_strings);
    read_part_from_file("edited_part.tbl", table_ints, table_strings, part_ints, part_strings);

    lineitem_table_size = table_ints[lineitem_ints[0]].size();
    part_table_size = table_ints[part_ints[0]].size();

    cout << lineitem_table_size << " " << part_table_size << endl;
    end = chrono::system_clock::now();
    diff = end-start;
    cout<<"Done! elapsed time: " << diff.count() <<"s\n\n";

    start = chrono::system_clock::now();
    cout <<"preprocessing...\n";

    preprocess_using_real_table();
//    preprocess_using_distribution_parameters();

    int quantity;
    string shipmode;
    string shipinstruct;
    int price;
    int discount;

    end = chrono::system_clock::now();
    diff = end-start;
    cout<<"Done! elapsed time: " << diff.count() <<"s\n\n";

    cout <<"Generating answer...\n";
    start = chrono::system_clock::now();

    for (int i = 0; i < lineitem_table_size; ++i) {

        shipmode = table_strings[l_shipmode_id][i];
        shipinstruct = table_strings[l_shipinstruct_id][i];

        if ((shipmode == "AIR" || shipmode == "REG_AIR") &&
            (shipinstruct == "DELIVER_IN_PERSON")) {

            quantity = table_ints[l_quantity_id][i];
            discount = table_ints[l_discount_id][i];
            price = table_ints[l_price_id][i];

            if ((1 <= quantity && quantity <= 11)) {
                num_of_real_table_acceptances[0]++;
            }
            if ((10 <= quantity && quantity <= 20)) {
                num_of_real_table_acceptances[1]++;
            }
            if ((20 <= quantity && quantity <= 30)) {
                num_of_real_table_acceptances[2]++;
            }
        }
    }

    double ans = 0;
    double acceptance = 0;

    //one of the following fragments should be commented. First one is used for prob method and second one is for dist method.

    //prob:
    double expected_price = (double)summations[l_price_id] / price_coefficient / lineitem_table_size;
    double expected_discount = (double)summations[l_discount_id] / discount_coefficient / lineitem_table_size;

    //dist:
//    double expected_price = params[l_price][0];
//    double expected_discount = params[l_discount][0];

    for (int i = 0; i < num_of_conditions; ++i){
        acceptance += dist_table_probs[i] * (double)num_of_real_table_acceptances[i];
        ans += dist_table_probs[i] * (double)num_of_real_table_acceptances[i]
                * (expected_price)
                * (1 - expected_discount);
    }

    cout <<"Acceptance percentage:" << acceptance/lineitem_table_size * 100 << endl;
    cout << "Revenue = "
         << ans << endl;
    end = chrono::system_clock::now();
    diff = end-start;
    cout<<"Done! elapsed time: " << diff.count() <<"s\n\n";
}