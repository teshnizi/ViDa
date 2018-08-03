//
// Created by teshnizi on 20/07/18.
//

#include <sstream>
#include <string>
#include <algorithm>

using namespace std;

enum {
    key = 0,
    quantity = 1,
    price = 2,
    discount = 3,
    ship_date = 4
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


struct lineitem{

    float attributes[5];    // 0.key  1.quantity 2.price 3.discount 4.ship_date

    lineitem(int item_key, int item_quantity, float item_extended_price, float item_discount, int item_ship_date){
        attributes[key] = item_key;
        attributes[ship_date] = item_ship_date;
        attributes[price] = item_extended_price;
        attributes[discount] = item_discount;
        attributes[quantity] = item_quantity;
    }

    lineitem(string csv){
        replace(csv.begin(), csv.end(), ',', ' ');
        istringstream iss(csv);
        iss >> attributes[key] >> attributes[quantity] >> attributes[price] >> attributes[discount] >> attributes[ship_date];
    }

    string to_str(){
        return "Key: " + to_string(attributes[key]) + "  quantity: " + to_string(attributes[quantity]) +
               "  price: " + to_string(attributes[price]) + "  discount: " + to_string(attributes[discount]) +
               "  ship date: " + days_to_date(attributes[ship_date]);
    }

};