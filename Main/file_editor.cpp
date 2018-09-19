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
#include "tuples2.h"
#include <vector>


using namespace std;

//Creates indices from csv file and stores them in "date_indices.txt" and "key_indices.txt"

void edit_lineitem_table(string name){
    ifstream fin(name);
    ofstream fout("edited_" + name);
    string line;

    while(getline(fin, line)){
        replace(line.begin(), line.end(), ' ', '_');
        replace(line.begin(), line.end(), '|', ' ');
        int orderkey, partkey, suppkey, quantity, linenumber;
        float discount, extendedprice, tax;
        char returnflag, linestatus;
        string shipdate, commitdate, receiptdate, shipinstruct, shipmode, z;
        istringstream iss(line);
        iss >> orderkey >> partkey >> suppkey >> linenumber >> quantity >> extendedprice >> discount >> tax >> returnflag >> linestatus >> shipdate
                >> commitdate >> receiptdate >> shipinstruct >> shipmode >> z;
        fout << orderkey << " " << partkey << " " << suppkey << " " << linenumber << " " << quantity << " " <<
             extendedprice << " " << discount << " " << tax << " " << returnflag << " " << linestatus << " " <<
             date_to_days(shipdate) << " " << date_to_days(commitdate) << " " << date_to_days(receiptdate) << " " <<
                shipinstruct << " " << shipmode << endl;
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
//        int l_partkey, size, retail_price;
//        string name, MFGR, brand, type, container, comment;
//        string l_shipdate, z;
//        istringstream iss(line);
//        iss >> l_partkey >> name >> MFGR >> brand >> type >> size >> container >> retail_price >> comment;

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


