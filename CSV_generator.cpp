
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


using namespace std;

const int test_size = 100007;


int main(){

    ofstream fout;
    fout.open("test_data.csv");
    vector<int> v;

    for (int i = 0; i < test_size; ++i) {
        v.push_back(i);
    }

    shuffle(v.begin(), v.end(), std::mt19937(std::random_device()()));

    for (int i = 0; i < test_size; ++i) {
        int extended_price = (rand()%100) * 5;
        float discount = (rand()%20)/40.0;
        int year = 1990 + (rand()%28);
        int month = (rand()%12) + 1;
        int day = rand()%30;
        int quantity = (rand()%100) * 50 + 50;

        string d = to_string(year) + "-";
        if ( month < 10) d = d + "0";
        d = d + to_string(month) + "-";
        if ( day < 10) d = d + "0";
        d = d + to_string(day);

        fout<<v[i]<<","<<extended_price<<","<<discount<<","<<d<<","<<quantity<<endl;
    }
    fout.close();


}
