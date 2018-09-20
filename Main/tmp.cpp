

#include <math.h>
#include <stdio.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

using namespace std;

const int max_table_num = 2;
const int max_table_size = 6000000;
const int hash_table_size = 1000000;
const int max_hashed_attributes = 3;
const int max_attribute_num = 15;
const int max_bucket_count = 40;
const int BUCKET_COUNT = 40;
const int discount_coefficient = 100;
const int tax_coefficient = 100;
const int price_coefficient = 10;
const int retailprice_coefficient = 100;

enum tables{
    lineitem,
    part
};

struct DataItem{
    int data;
    int key;
};

typedef struct DataItem DataItem;

int default_bucket[max_table_num][max_attribute_num][max_bucket_count];
int default_minimum[max_table_num][max_attribute_num];
int default_maximum[max_table_num][max_attribute_num];
int default_bucket_width[max_table_num][max_attribute_num];
int default_table_size[max_table_num];
int table_size[max_table_num];
int int_table[max_table_num][max_attribute_num][max_table_size];
string string_table[max_table_num][max_attribute_num][max_table_size];
DataItem* hashArray[max_hashed_attributes][hash_table_size * 2];
struct DataItem* dummyItem;

enum l_table{
    l_orderkey_id = 0,
    l_quantity_id ,
    l_price_id ,
    l_discount_id ,
    l_shipdate_id ,
    l_partkey_id ,
    l_suppkey_id ,
    l_linenumber_id ,
    l_tax_id ,
    l_returnflag_id ,
    l_linestatus_id ,
    l_commitdate_id ,
    l_receiptdate_id ,
    l_shipinstruct_id ,
    l_shipmode_id,
};

enum p_table{
    p_partkey_id = 0,
    p_name_id,
    p_mfgr_id,
    p_brand_id,
    p_type_id,
    p_size_id,
    p_container_id,
    p_retailprice_id,
    p_comment_id,
};


void read_lineitems_from_file(string name,
                              int *int_attributes, int num_of_int_attributes, int *string_attributes, int num_of_string_attributes){

    for (int i = 0; i < max_attribute_num; ++i) {
            default_minimum[lineitem][i] = 1e8;
            default_maximum[lineitem][i] = -1e8;
    }
    ifstream fin;
    fin.open(name);
    string line;
    int int_tmp[20];
    float float_tmp[20];
    char char_tmp[20];
    string string_tmp[20];
    int iterator = 0;
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

        for (int i = 0; i < num_of_int_attributes; ++i) {
            int_table[lineitem][int_attributes[i]][iterator] = int_tmp[int_attributes[i]];
            default_maximum[lineitem][int_attributes[i]] =
                    max(default_maximum[lineitem][int_attributes[i]], int_table[lineitem][int_attributes[i]][iterator]);
            default_minimum[lineitem][int_attributes[i]] =
                    min(default_minimum[lineitem][int_attributes[i]], int_table[lineitem][int_attributes[i]][iterator]);
        }

        // table_ints[int_attributes[i]].push_back(int_tmp[int_attributes[i]]);
        for (int i = 0; i < num_of_string_attributes; ++i)
            string_table[lineitem][string_attributes[i]][iterator] = string_tmp[string_attributes[i]];
        // table_strings[string_attributes[i]].push_back(string_tmp[string_attributes[i]]);
        iterator++;
    }
    table_size[lineitem] = iterator;

    for (int i = 0; i < max_table_size; i++){
        
    }
}



void read_part_from_file(string name,
                         int *int_attributes, int num_of_int_attributes, int *string_attributes, int num_of_string_attributes){

    for (int i = 0; i < max_attribute_num; ++i) {
        default_minimum[part][i] = 1e8;
        default_maximum[part][i] = -1e8;
    }

    ifstream fin;
    fin.open(name);
    string line;
    int int_tmp[20];
    float float_tmp[20];
    char char_tmp[20];
    string string_tmp[20];
    int iterator = 0;
    while (getline(fin,line,'\n')) {
        istringstream iss(line);
        iss >> int_tmp[p_partkey_id] >> string_tmp[p_name_id] >> string_tmp[p_mfgr_id] >> string_tmp[p_brand_id] >> string_tmp[p_type_id] >> int_tmp[p_size_id] >>
            string_tmp[p_container_id] >> float_tmp[p_retailprice_id] >> string_tmp[p_comment_id];
        int_tmp[p_retailprice_id] = retailprice_coefficient * float_tmp[p_retailprice_id];

        for (int i = 0; i < num_of_int_attributes; ++i) {
            int_table[part][int_attributes[i]][iterator] = int_tmp[int_attributes[i]];
            default_maximum[part][int_attributes[i]] =
                    max(default_maximum[part][int_attributes[i]], int_table[part][int_attributes[i]][iterator]);
            default_minimum[part][int_attributes[i]] =
                    min(default_minimum[part][int_attributes[i]], int_table[part][int_attributes[i]][iterator]);
        }

        for (int i = 0; i < num_of_string_attributes; ++i) {
            string_table[part][string_attributes[i]][iterator] = string_tmp[string_attributes[i]];
        }
        iterator++;
    }
    table_size[part] = iterator;
}


double limit(int bucket[], int minimum, int maximum, int l, int h){
    int bucket_size = ceil((float)(maximum + 1 - minimum)/(BUCKET_COUNT));
    int total = 0;
    int left = 0;
    for (int i = 0; i < BUCKET_COUNT; ++i) {
        int lo = minimum + bucket_size * i;
        int hi = lo + bucket_size;
        total += bucket[i];
        if (lo >= h || hi < l)
            bucket[i] = 0;
        else if ( l <= lo && hi < h)
            left += bucket[i];
        else{
            left += bucket[i];
            bucket[i] = bucket[i] * ((lo < l) * (hi - l) + (lo >= l) * (h - lo))/(hi - lo);
            left -= bucket[i];
        }
    }
    return (double)left/total;
}


double string_limit(int bucket[], bool valid[]){
    int total = 0;
    int left = 0;
    for (int i = 0; i < BUCKET_COUNT; ++i) {
        total += bucket[i];
        if(!valid[i])
            bucket[i] = 0;
        left += bucket[i];
    }
    return (double)left/total;
}

int hashCode(int key) {
    return key % hash_table_size;
}

struct DataItem *search(int id, int key) {
    //get the hash
    int hashIndex = hashCode(key);

    //move in array until an empty
    while(hashArray[id][hashIndex] != NULL) {

        if(hashArray[id][hashIndex]->key == key)
            return hashArray[id][hashIndex];

        //go to next cell
        ++hashIndex;

        //wrap around the table
        hashIndex %= hash_table_size;
    }
    return NULL;
}

void insert(int id, int key,int data) {
    struct DataItem *item = (struct DataItem*) malloc(sizeof(struct DataItem));
    item->data = data;
    item->key = key;

    //get the hash
    int hashIndex = hashCode(key);

    //move in array until an empty or deleted cell
    while(hashArray[id][hashIndex] != NULL && hashArray[id][hashIndex]->key != -1) {
        //go to next cell
        ++hashIndex;

        //wrap around the table
        hashIndex %= hash_table_size;
    }
    hashArray[id][hashIndex] = item;
}

struct DataItem* remove(int id, struct DataItem* item) {
    int key = item->key;

    //get the hash
    int hashIndex = hashCode(key);

    //move in array until an empty
    while (hashArray[hashIndex] != NULL) {

        if (hashArray[id][hashIndex]->key == key) {
            struct DataItem *temp = hashArray[id][hashIndex];

            //assign a dummy item at deleted position
            hashArray[id][hashIndex] = dummyItem;
            return temp;
        }

        //go to next cell
        ++hashIndex;

        //wrap around the table
        hashIndex %= hash_table_size;
    }

    return NULL;
}
