/*
Created by teshnizi on 31/07/18.
finds maximum and minimum of data and starts generating uniformly distributed random numbers and selecting those
which satisfy query conditions.
*/

#include <iostream>
#include <fstream>
#include "tuples.h"
#include <math.h>
#include "chrono"

int main(){

    auto start = chrono::system_clock::now();
    auto end = chrono::system_clock::now();
    chrono::duration<double> diff;

    float min_price=1e9, max_price=0,
            min_discount=1e9, max_discount=0;

    int min_quantity=1e7, max_quantity=0,
        min_ship_date=1e7, max_ship_date=0;


    cout<<"\nreading table from file in order to find min and max...\n";
    ifstream fin;
    fin.open("edited_lineitem.tbl");
    string line;
    int orderkey, quantity, ship_date;
    float discount, extended_price;
    int table_size = 0;

    while (getline(fin,line,'\n')) {

        table_size++;
        istringstream iss(line);
        iss >> orderkey >> quantity >> extended_price >> discount >> ship_date;

        min_discount = min(min_discount, discount);
        max_discount = max(max_discount, discount);

        min_price = min(min_price, extended_price);
        max_price = max(max_price, extended_price);

        min_ship_date = min(min_ship_date, ship_date);
        max_ship_date = max(max_ship_date, ship_date);

        min_quantity = min(min_quantity, quantity);
        max_quantity = max(max_quantity, quantity);
    }

    end = chrono::system_clock::now();
    diff = end-start;
    cout<<"Done! elapsed time: " << diff.count() <<"s\n\n";




    start = chrono::system_clock::now();
    srand(time(NULL));
    cout << "Generating random data...\n";

    float price_range = max_price - min_price;
    float discount_range = max_discount - min_discount;
    int ship_date_range = max_ship_date - min_ship_date;
    int quantity_range = max_quantity - min_quantity;
    int start_date = date_to_days("1994-01-01");
    int end_date = date_to_days("1995-01-01");
    int ans = 0;

//    cout << min_discount << " " << max_discount << " " << min_quantity << " " << max_quantity << " " << min_price << " " << max_price << " " << min_ship_date << " " << max_ship_date << endl;
    for (int i = 0; i < table_size; ++i){
        extended_price = (rand()/(float)RAND_MAX) * price_range + min_price;
        discount = (rand()/(float)RAND_MAX) * discount_range + min_discount;
        ship_date = (rand()/(float)RAND_MAX) * ship_date_range + min_ship_date;
        quantity = (rand()/(float)RAND_MAX) * quantity_range + min_quantity;

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
