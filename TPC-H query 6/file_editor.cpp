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

void edit_lineitem_table(string name){
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
    cout << "original TPC-H table " + name + " was edited and saved as edited_" + name << endl;
}

void edit_part_table(string name){
    ifstream fin(name);
    ofstream fout("edited_" + name);
    string line;

    while(getline(fin, line)){
        replace(line.begin(), line.end(), ' ', '_');
        replace(line.begin(), line.end(), '|', ' ');
        fout << line <<endl;
//        int partkey, size, retail_price;
//        string name, MFGR, brand, type, container, comment;
//        string shipdate, z;
//        istringstream iss(line);
//        iss >> partkey >> name >> MFGR >> brand >> type >> size >> container >> retail_price >> comment;

    }

    fin.close();
    fout.close();
    cout << "original TPC-H table " + name + " was edited and saved as edited_" + name << endl;

}
//Generic type could also be used for index readers, by the way due to limited number of types used in code,
// I preferred separate functions.


int main(){
    edit_lineitem_table("lineitem.tbl");
    edit_part_table("part.tbl");
}