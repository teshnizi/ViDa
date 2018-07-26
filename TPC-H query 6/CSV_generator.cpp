
/*
 * Generates a test CSV file
 * Each line contains a tuple in the following format:
 * PRIMARY_KEY(order key, part key, sup key), EXTENDED_PRICE, DISCOUNT, SHIP_DATE, QUANTITY
 */

#include <iostream>
#include <random>
#include <ctime>
#include <set>
#include <fstream>
#include <vector>
#include <algorithm>
#include <chrono>
#include "tuples.h"


using namespace std;

const int test_size = 60000007;

const float discount_mean = 0.05;
const float discount_var = 0.01;

const float extended_price_mean = 50;
const float extended_price_var = 10;

const float quantity_mean = 20;
const float quantity_var = 5;

int main(){

    cout<<endl;
    auto start = chrono::system_clock::now();

    ofstream fout;
    fout.open("test_data.csv");
    vector<int> v;

    for (int i = 0; i < test_size; ++i) {
        v.push_back(i);
    }

    shuffle(v.begin(), v.end(), std::mt19937(std::random_device()()));

    default_random_engine generator;
    normal_distribution<double> distribution(5.0,2.0);
    normal_distribution<double> price_dist(extended_price_mean, extended_price_var);
    normal_distribution<double> quantity_dist(quantity_mean, quantity_var);
    normal_distribution<float> discount_dist(discount_mean, discount_var);


    for (int i = 0; i < test_size; ++i) {

        int extended_price = int(price_dist(generator));
        if( extended_price < 1 ) extended_price = 1;

        float discount = discount_dist(generator);
        if( discount < 0 ) discount = 0;


        int year = 90 + (rand()%28);
        int month = (rand()%12) + 1;
        int day = (rand()%30) + 1;


        int quantity = int(quantity_dist(generator));
        if ( quantity < 1 ) quantity = 1;

        int d = year * 365 + month * 30 + day;

        fout<<v[i]<<","<<extended_price<<","<<discount<<","<<d<<","<<quantity<<endl;
    }
    fout.close();
    auto end = chrono::system_clock::now();
    chrono::duration<double> diff = end - start;

    cout<<"Done! elapsed time: " << diff.count() <<"s\n\n";
}
