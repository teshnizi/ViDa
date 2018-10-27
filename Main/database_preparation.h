#include <math.h>
#include <stdio.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <stdbool.h>

using namespace std;

const int max_table_num = 2;
const int max_table_size = 6000000;
const int hash_table_size = 1000000;
const int max_hashed_attributes = 3;
const int max_attribute_num = 15;
const int max_bucket_count = 40;
const int BUCKET_COUNT = 100;
const int discount_coefficient = 100;
const int tax_coefficient = 100;
const int price_coefficient = 10;
const int retailprice_coefficient = 100;

enum tables{
    lineitem,
    part
};

struct Histogram{
    float bucket[100];
    float minimum;
    float maximum;
    int bucket_count = -1;
    float bucket_size;
};

typedef struct Histogram Histogram;

struct DataItem{
    int data;
    int key;
};

typedef struct DataItem DataItem;


Histogram default_histogram[max_table_num][max_attribute_num];

int default_table_size[max_table_num];
int table_size[max_table_num];
int int_table[max_table_num][max_attribute_num][max_table_size];
string string_table[max_table_num][max_attribute_num][max_table_size];
DataItem* hashArray[max_hashed_attributes][hash_table_size * 2];
struct DataItem* dummyItem;

enum hash_id{
    p_partkey_hash_id = 0,
    groupby_hash_id,
};

enum l_table{
    l_orderkey_id = 0,
    l_quantity_id ,
    l_price_id ,
    l_discount_id = 3,
    l_shipdate_id ,
    l_partkey_id ,
    l_suppkey_id = 6,
    l_linenumber_id ,
    l_tax_id ,
    l_returnflag_id = 9,
    l_linestatus_id ,
    l_commitdate_id ,
    l_receiptdate_id = 12,
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
            default_histogram[lineitem][i].minimum = 1e8;
            default_histogram[lineitem][i].maximum = -1e8;
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
            default_histogram[lineitem][int_attributes[i]].maximum =
                    max(default_histogram[lineitem][int_attributes[i]].maximum, (float)int_table[lineitem][int_attributes[i]][iterator]);
            default_histogram[lineitem][int_attributes[i]].minimum =
                    min(default_histogram[lineitem][int_attributes[i]].minimum, (float)int_table[lineitem][int_attributes[i]][iterator]);
        }

        // table_ints[int_attributes[i]].push_back(int_tmp[int_attributes[i]]);
        for (int i = 0; i < num_of_string_attributes; ++i)
            string_table[lineitem][string_attributes[i]][iterator] = string_tmp[string_attributes[i]];
        // table_strings[string_attributes[i]].push_back(string_tmp[string_attributes[i]]);
        iterator++;
    }
    table_size[lineitem] = iterator;
}



void read_part_from_file(string name,
                         int *int_attributes, int num_of_int_attributes, int *string_attributes, int num_of_string_attributes){
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
            default_histogram[part][int_attributes[i]].maximum =
                    max(default_histogram[part][int_attributes[i]].maximum, (float)int_table[part][int_attributes[i]][iterator]);
            default_histogram[part][int_attributes[i]].minimum =
                    min(default_histogram[part][int_attributes[i]].minimum, (float)int_table[part][int_attributes[i]][iterator]);
        }

        for (int i = 0; i < num_of_string_attributes; ++i) {
            string_table[part][string_attributes[i]][iterator] = string_tmp[string_attributes[i]];
        }
        iterator++;
    }
    table_size[part] = iterator;
}

//reads histograms from file:
void read_histogram_defaults_from_file(string file_name){
    ifstream fin;
    fin.open(file_name);
    string line;
    while(getline(fin, line,'\n')){ // each line comes in the following format:
        // table_id     attribute_id     bucket_count    minimum     maximum     bucket[0]  bucket[1]   ... bucket[bucket_count-1]
        istringstream iss(line);
        int table_id, attribute_id;
        iss >> table_id >> attribute_id;
        int bc, mn, mx;
        iss >> bc >> mn >> mx;
        default_histogram[table_id][attribute_id].bucket_count = bc;
        default_histogram[table_id][attribute_id].minimum = mn;
        default_histogram[table_id][attribute_id].maximum = mx;
        default_histogram[table_id][attribute_id].bucket_size = (mx - mn) / bc;

        // default_histogram[table_id][attribute_id].bucket = (float*) malloc(bc * sizeof(int));
        for (int i = 0; i < bc; i++)
            iss >> default_histogram[table_id][attribute_id].bucket[i];
    }
}

//
// double limit(int bucket[], int minimum, int maximum, int l, int h){
//     int bucket_size = ceil((float)(maximum + 1 - minimum)/(BUCKET_COUNT));
//     int total = 0;
//     int left = 0;
//     for (int i = 0; i < BUCKET_COUNT; ++i) {
//         int lo = minimum + bucket_size * i;
//         int hi = lo + bucket_size;
//         total += bucket[i];
//         if (lo >= h || hi < l)
//             bucket[i] = 0;
//         else if ( l <= lo && hi < h)
//             left += bucket[i];
//         else{
//             left += bucket[i];
//             bucket[i] = bucket[i] * ((lo < l) * (hi - l) + (lo >= l) * (h - lo))/(hi - lo);
//             left -= bucket[i];
//         }
//     }
//     return (double)left/total;
// }


double limit(Histogram *h, float m, float M){
    double bs = (h->maximum-h->minimum)/h->bucket_count;
    double total = 0;
    double remaining = 0;
    for (int i =0; i < h->bucket_count; i++){
        double lo = h->minimum + i * bs;
        double hi = lo + bs;
        total += h->bucket[i];
        if (lo >= M || hi < m)
            h->bucket[i] = 0;
        else if ( m <= lo && hi < M){
            remaining += h->bucket[i];
        }
        else{
            if (lo < m)
                h->bucket[i] *= (hi - m)/(hi - lo);
            else
                h->bucket[i] *= (M - lo)/(hi - lo);
            remaining += h->bucket[i];
        }
    }
    return remaining/total;
}


//
// double string_limit(int bucket[], bool valid[]){
//     int total = 0;
//     int left = 0;
//     for (int i = 0; i < BUCKET_COUNT; ++i) {
//         total += bucket[i];
//         if(!valid[i])
//             bucket[i] = 0;
//         left += bucket[i];
//     }
//     return (double)left/total;
// }

double string_limit(Histogram *h, int id){
    int total = 0;
    int remaining = 0;
    for ( int i = 0; i < h->bucket_count; ++i)
        total += h->bucket[id];
    remaining = total - h->bucket[id];
    h->bucket[id] = 0;
    if(total == 0){
        cout << "ZERO HIST ERROR!\n";
    }
    return (float)remaining/total;
}

int hashCode(int key) {
    return key % hash_table_size;
}

int total_count(Histogram *h){
    int ret = 0;
    for (int i = 0 ; i < h->bucket_count; i++){
        ret += h->bucket[i];
    }
    return ret;
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

void copy_hist(Histogram* dest, Histogram* source){
    dest->minimum = source->minimum;
    dest->maximum = source->maximum;
    dest->bucket_size = source->bucket_size;
    dest->bucket_count = source->bucket_count;
    // if (dest->bucket == nullptr)
    //     free(dest->bucket);
    // dest->bucket = (float*)malloc(dest->bucket_count * sizeof(int));
    for (int i = 0 ; i < source->bucket_count; ++i){
        dest->bucket[i] = source->bucket[i];
    }
}



double data_data_sum_data(double x, double y){
    return x + y;
}

double data_hist_sum_data(double x, Histogram h){
    double mean = 0;
    double count = 0;
    double bs = (h.maximum - h.minimum)/h.bucket_count;
    for (int i = 0; i < h.bucket_count; ++i) {
        count += h.bucket[i];
        mean += h.bucket[i] * (h.minimum + bs * i + bs/2);
    }
    mean /= count;
    return mean * x;
}

double hist_data_sum_data(Histogram h, double x){
    return data_hist_sum_data(x, h);
}

double hist_hist_sum_data(Histogram a, Histogram b){
    double val1 = 0;
    double val2 = 0;
    double count1 = 0;
    double count2 = 0;
    double summation = 0;
    double bs1 = (a.maximum - a.minimum)/a.bucket_count;
    double bs2 = (b.maximum - b.minimum)/b.bucket_count;
    for (int i = 0; i < a.bucket_count; ++i) {
        count1 += a.bucket[i];
    }
    for (int i = 0; i < b.bucket_count; ++i) {
        count2 += b.bucket[i];
    }
    double mlt = count1 * count2;
    for (int i = 0; i < a.bucket_count; ++i) {
        for (int j = 0; j < b.bucket_count; ++j) {
            val1 = a.bucket[i] * (a.minimum + bs1 * i + bs1/2);
            val2 = b.bucket[j] * (b.minimum + bs2 * j + bs2/2);
            summation += (val1 * val2) / mlt;
        }
    }
    return summation;
}

double data_data_sub_data(double x, double y){
    return x - y;
}

double data_data_mult_data(double x, double y){
    return x * y;
}

double hist_hist_mult_data(Histogram a, Histogram b){
    double mean1 = 0;
    double mean2 = 0;
    double count1 = 0;
    double count2 = 0;
    double bs1 = (a.maximum - a.minimum)/a.bucket_count;
    double bs2 = (b.maximum - b.minimum)/b.bucket_count;
    for (int i = 0; i < a.bucket_count; ++i) {
        count1 += a.bucket[i];
        mean1 += a.bucket[i] * (a.minimum + bs1 * i + bs1/2);
    }
    for (int i = 0; i < b.bucket_count; ++i) {
        count2 += b.bucket[i];
        mean2 += b.bucket[i] * (b.minimum + bs2 * i + bs2/2);
    }
    mean1 /= count1;
    mean2 /= count2;
    return mean1 * mean2;
}

double data_hist_mult_data(double x, Histogram h){
    double mean = 0;
    double count = 0;
    double bs = (h.maximum - h.minimum)/h.bucket_count;
    for (int i = 0; i < h.bucket_count; ++i) {
        count += h.bucket[i];
        mean += h.bucket[i] * (h.minimum + bs * i + bs/2);
    }
    mean /= count;
    return mean * x;
}

double hist_data_mult_data(Histogram h, double x){
    data_hist_mult_data(x, h);
}
