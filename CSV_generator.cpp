
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

using namespace std;

const int test_size = 100007;


int main(){

    ofstream fout;
    fout.open("test_data.csv");

    for (int i = 0; i < test_size; ++i) {
        int extended_price = (rand()%100) * 5;
        float discount = (rand()%20)/40.0;
        int year = 1990 + (rand()%28);
        int month = (rand()%12) + 1;
        int day = rand()%30;
        int quantity = (rand()%100) * 50 + 50;
        fout<<i<<","<<extended_price<<","<<discount<<","<<year<<"-"<<month<<"-"<<day<<","<<quantity<<endl;
    }
    fout.close();
}
