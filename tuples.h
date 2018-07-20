//
// Created by teshnizi on 20/07/18.
//

using namespace std;

struct date{

    short year;
    short month;
    short day;
    date(){
        year = 1998;
        month = 8;
        day = 10;
    }

    date(short d_year, short d_month, short d_day){
        year= d_year;
        month = d_month;
        day = d_day;
    }

    friend bool operator<(date x, date y){
        if ( x.year != y.year)
            return x.year<y.year;
        if ( x.month != y.month)
            return x.month<y.month;
        return x.day<y.day;
    }

    friend bool operator==(date x, date y){
        return ((x.year == y.year) && (x.month == y.month) && (x.day == y.day));
    }

};


struct line_item{

    int key;
    date ship_date;
    int extended_price;
    float discount;
    int quantity;

    line_item(int item_key, date item_ship_date, int item_extended_price, float item_discount, int item_quantity){
        key = item_key;
        ship_date = item_ship_date;
        extended_price = item_extended_price;
        discount = item_discount;
        quantity = item_quantity;
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

};