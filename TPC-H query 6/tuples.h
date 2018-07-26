//
// Created by teshnizi on 20/07/18.
//

#include <sstream>
#include <string>
#include <algorithm>

using namespace std;


const int month_days_acc[] = {31, 31+28, 31+28+31, 31+28+31+30, 31+28+31+30+31,
                              31+28+31+30+31+30, 31+28+31+30+31+30+31, 31+28+31+30+31+30+31+31,
                              31+28+31+30+31+30+31+31+30, 31+28+31+30+31+30+31+31+30+31,
                              31+28+31+30+31+30+31+31+30+31+30, 31+28+31+30+31+30+31+31+30+31+30+31};


string days_to_string(int days){
    int year = (days / 365);
    days -= year * 365;
    int month = (lower_bound(month_days_acc,month_days_acc+12, days+1) - month_days_acc);
//    cout << month << endl;
    if ( month > 0 )
        days -= month_days_acc[month-1];
    return to_string(year + 1900) + "-" + to_string(month + 1) + "-" + to_string(days + 1);
}

int string_date_to_days(string date){
    replace(date.begin(), date.end(), '-', ' ');
    istringstream iss(date);
    int year, month, day, days;
    iss >> year >> month >> day;
    days = (year-1900) * 365;
    (month > 1)?days+=month_days_acc[month-2]:1+1;
    days += day - 1;
    return days;
}


struct line_item{

    int key;
    int ship_date;
    int extended_price;
    float discount;
    int quantity;

    line_item(int item_key, int item_ship_date, int item_extended_price, float item_discount, int item_quantity){
        key = item_key;
        ship_date = item_ship_date;
        extended_price = item_extended_price;
        discount = item_discount;
        quantity = item_quantity;
    }

    line_item(string csv){
        replace(csv.begin(), csv.end(), ',', ' ');
        istringstream iss(csv);
        iss >> key >> extended_price >> discount >> ship_date >> quantity;
    }

    friend bool operator<(line_item x, line_item y){
        return x.key < y.key;
    }

    friend bool operator==(line_item x, line_item y){
        return (x.key == y.key
                && x.ship_date == y.ship_date
                && x.extended_price == y.extended_price
                && x.discount == y.discount
                && x.quantity == y.quantity);
    }

    string to_str(){
        return "Key: " + to_string(key) + "   Ship date: " + days_to_string(ship_date) +
               "  price: " + to_string(extended_price) + "  discount: " + to_string(discount) + " quantity: " + to_string(quantity);
    }

};