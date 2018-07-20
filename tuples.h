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

};
