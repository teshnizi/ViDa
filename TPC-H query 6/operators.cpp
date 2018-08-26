//
// Created by teshnizi on 24/08/18.
//

#include "data_operators.h"

using namespace std;

vector<int> table_ints[ENUM_COUNT];
vector<string> table_strings[ENUM_COUNT];

vector<int> lineitem_ints;
vector<int> lineitem_strings;

Histogram hist_ints[ENUM_COUNT];

int lineitem_table_size;

pair<int, int> condition_ranges[ENUM_COUNT];

void preprocess(){
    lineitem_ints.push_back        (l_partkey);
    lineitem_ints.push_back        (l_quantity);
    lineitem_ints.push_back        (l_price);
    lineitem_ints.push_back        (l_discount);
    lineitem_ints.push_back        (l_tax);
    lineitem_strings.push_back     (l_shipmode);
    lineitem_strings.push_back     (l_shipinstruct);
}

int main(){

    try {
        preprocess();
        read_lineitem_from_file("edited_lineitem.tbl", table_ints, table_strings, lineitem_ints, lineitem_strings);
        lineitem_table_size = table_ints[lineitem_ints[0]].size();
        for (int i = 0; i < ENUM_COUNT; ++i) {
            if (table_ints[i].size() > 0)
                hist_ints[i] = Histogram(&table_ints[i], 40);
        }
        set<string> attributes;
//    attributes.insert("quantity");
//    attributes.insert("price");

        vector<string> variables;
        variables.push_back("shipdate");
        variables.push_back("discount");
        variables.push_back("quantity");
        vector<pair<int, int>> range;
        range.push_back(make_pair(date_to_days("1994-01-01"), date_to_days("1995-01-01")));
        range.push_back(make_pair(5, 7));
        range.push_back(make_pair(0, 24));

        Node root = Node();


        //on real data:
        cout<<"==============================================\n============ Code For Real Table ======"
              "=======\n==============================================\n\n";
        MapNode mapNode = MapNode(&root, "(price * discount)", "revenue");

        DataSelectNode dataSelectNode = DataSelectNode(&mapNode, variables, range);
        DataScanNode dataScanNode = DataScanNode(&dataSelectNode, "lineitem", lineitem_table_size);

        dataSelectNode.set_child(&dataScanNode);
        mapNode.set_child(&dataSelectNode);
        root.set_child(&mapNode);

        root.produce(attributes);


        //on histograms:
        cout<<"==============================================\n============ Code For Histograms ======="
              "======\n==============================================\n\n";
        HistSelectNode histSelectNode = HistSelectNode(&mapNode, variables, range);
        HistScanNode histScanNode = HistScanNode(&histSelectNode);

        histSelectNode.set_child(&histScanNode);
        mapNode.set_child(&histSelectNode);
        root.set_child(&mapNode);

        root.produce(attributes);

    }

    catch (const char* massage){
        cerr << massage << endl;
    }
}