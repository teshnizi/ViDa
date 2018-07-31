//
// Created by teshnizi on 31/07/18.
// Generates data based on original data approximate distributions
//

#include <iostream>
#include <fstream>
#include "tuples.h"
#include <math.h>
#include "chrono"
#include <vector>
#include "statistics.h"
#include <algorithm>


const int segment_count = 10;


int main(){

    auto start = chrono::system_clock::now();
    auto end = chrono::system_clock::now();
    chrono::duration<double> diff;

    cout<<"\nreading table from file in order to find min and max...\n";
    ifstream fin;
    fin.open("edited_lineitem.tbl");
    string line;
    int orderkey, quantity, ship_date;
    float discount, extended_price;
    int table_size = 0;

    vector <float> extended_prices, discounts;
    vector <int> ship_dates, quantities;

    while (getline(fin,line,'\n')) {
        table_size++;
        istringstream iss(line);
        iss >> orderkey >> quantity >> extended_price >> discount >> ship_date;

        extended_prices.push_back(extended_price);
        discounts.push_back(discount);
        ship_dates.push_back(ship_date);
        quantities.push_back(quantity);
    }

    end = chrono::system_clock::now();
    diff = end-start;
    cout<<"Done! elapsed time: " << diff.count() <<"s\n\n";


    start = chrono::system_clock::now();
    cout<<"finding histograms...\n\n";

    sort(extended_prices.begin(), extended_prices.end());
    sort(quantities.begin(), quantities.end());
    sort(discounts.begin(), discounts.end());
    sort(ship_dates.begin(), ship_dates.end());


    float ship_date_block_size;
    float quantity_block_size;
    float discount_block_size;
    float price_block_size;

    float ship_date_lower_bound;
    float discount_lower_bound;
    float price_lower_bound;
    float quantity_lower_bound;

    vector<int> price_hist = quantize(extended_prices, segment_count, price_block_size, price_lower_bound);
    vector<int> discount_hist = quantize(discounts, segment_count, discount_block_size, discount_lower_bound);
    vector<int> quantity_hist = quantize(quantities, segment_count, quantity_block_size, quantity_lower_bound);
    vector<int> ship_date_hist = quantize(ship_dates, segment_count, ship_date_block_size, ship_date_lower_bound);


    //converting histograms to cumulative histograms:
    for (int i = 1; i < segment_count; ++i) {
        price_hist[i] += price_hist[i-1];
        discount_hist[i] += discount_hist[i-1];
        quantity_hist[i] += quantity_hist[i-1];
        ship_date_hist[i] += ship_date_hist[i-1];
    }

    end = chrono::system_clock::now();
    diff = end-start;
    cout<<"Done! elapsed time: " << diff.count() <<"s\n\n";


    start = chrono::system_clock::now();
    srand(time(NULL));
    cout << "Generating random data...\n";

    int start_date = date_to_days("1994-01-01");
    int end_date = date_to_days("1995-01-01");
    int ans = 0;

//    cout << min_discount << " " << max_discount << " " << min_quantity << " " << max_quantity << " " << min_price << " " << max_price << " " << min_ship_date << " " << max_ship_date << endl;
    for (int i = 0; i < table_size; ++i){

        extended_price = get_random_data_by_hist(price_hist, price_block_size, price_lower_bound);
        discount = get_random_data_by_hist(discount_hist, discount_block_size, discount_lower_bound);
        ship_date = get_random_data_by_hist(ship_date_hist, ship_date_block_size, ship_date_lower_bound);
        quantity = get_random_data_by_hist(quantity_hist, quantity_block_size, quantity_lower_bound);

//        cout << quantity << " " << discount << " " << ship_date << " " << extended_price << endl;
        if ( start_date <= ship_date && ship_date < end_date)
            if ( quantity < 24)
                if ( (0.06 - 0.01) < discount && discount < (0.06 + 0.01) )
                    ans += extended_price * discount;
    }

    cout<<"Total revenue: " << ans<<endl;
    end = chrono::system_clock::now();
    diff = end-start;
    cout<<"Done! elapsed time: " << diff.count() <<"s\n\nOriginal table size: " << table_size << endl;
}