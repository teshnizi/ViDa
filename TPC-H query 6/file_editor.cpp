//
// Created by teshnizi on 20/07/18.
//


#include <iostream>
#include <fstream>
#include <set>
#include <string>
#include <algorithm>
#include <sstream>
#include <utility>
#include "tuples.h"
#include <vector>


using namespace std;

//Creates indices from csv file and stores them in "date_indices.txt" and "key_indices.txt"

void tbl_to_space_separated(string name){
    ifstream fin(name);
    ofstream fout("edited_" + name);
    string line;

    while(getline(fin, line)){
        replace(line.begin(), line.end(), '|', ' ');
        int orderkey, quantity, x;
        float discount, extendedprice, y;
        string shipdate, z;
        istringstream iss(line);
        iss >> orderkey >> x >> x >> x >> quantity >> extendedprice >> discount >> y >> z >> z >> shipdate;
        fout << orderkey << " " << quantity << " " << extendedprice << " " << discount << " " << date_to_days(shipdate) << endl;
    }

    fin.close();
    fout.close();
}
//Generic type could also be used for index readers, by the way due to limited number of types used in code,
// I preferred separate functions.


int main(){
    tbl_to_space_separated("lineitem.tbl");
}