//
// Created by teshnizi on 24/08/18.
//

#include "operators.h"

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
//        set<string> attributes;
//    attributes.insert("quantity");
//    attributes.insert("price");


        set<Attribute> attributes;

//        attributes.insert( attribute("lineitem", "discount"));

        attributes.insert(Attribute("lineitem", "l_discount"));
        attributes.insert(Attribute("lineitem", "l_partkey"));
        attributes.insert(Attribute("lineitem", "l_quantity"));
        attributes.insert(Attribute("part", "p_partkey"));
        attributes.insert(Attribute("part", "p_size"));

//        vector<string> variables;
//        variables.push_back("shipdate");
//        variables.push_back("discount");
//        variables.push_back("quantity");
//        vector<pair<int, int>> range;
//        range.push_back(make_pair(date_to_days("1994-01-01"), date_to_days("1995-01-01")));
//        range.push_back(make_pair(5, 7));
//        range.push_back(make_pair(0, 24));

        Node root = Node("Root");

        //on real data:
        cout<<"==============================================\n============ Code For Real Table ======"
              "=======\n==============================================\n\n";
//        MapNode mapNode = MapNode(&root, "(price * discount)", "revenue");

//        DataSelectNode dataSelectNode = DataSelectNode(&mapNode, variables, range);
//        DataScanNode dataScanNode = DataScanNode(&dataSelectNode, "lineitem", lineitem_table_size);
//
//        dataSelectNode.set_child(&dataScanNode);
//        mapNode.set_child(&dataSelectNode);
//        root.set_child(&mapNode);
//
//        root.produce(&attributes);

        //on histograms:
        cout<<"==============================================\n============ Code For Histograms ======="
              "======\n==============================================\n\n";
        cout<<"int default_bucket[max_table_num][max_attribute_num][max_bar_count];\n";
        cout<<"int default_minimum[max_table_num][max_attribute_num];\n";
        cout<<"int default_maximum[max_table_num][max_attribute_num];\n";
        cout<<"int default_bucket_width[max_table_num][max_attribute_num];\n";
        cout<<"int default_table_size[max_table_num];\n";
        cout<<"int table_size[max_table_num];\n";

        cout << "\ndouble limit(int bars[], int minimum, int maximum, int l, int h){\n"
                "\tint bucket_size = ceil((float)(maximum + 1 - minimum)/(bucket_count));\n"
                "\tint total = 0;\n"
                "\tint left = 0;\n"
                "\tfor (int i = 0; i < bucket_count; ++i) {\n"
                "        int lo = minimum + bucket_size * i;\n"
                "\t\tint hi = lo + bucket_size;\n"
                "\t\ttotal += bar[i];\n"
                "\t\tif (lo >= h || hi < l)\n"
                "\t\t\tbar[i] = 0;\n"
                "        else if ( l <= lo && hi < h)\n"
                "            left += bar[i];\n"
                "        else{\n"
                "\t\t\tleft += bar[i];\n"
                "            bar[i] = bar[i] * ((lo < l) * (hi - l) + (lo >= l) * (h - lo))/(hi - lo);\n"
                "\t\t\tleft -= bar[i];\n"
                "        }\n"
                "\t}\n"
                "\treturn (double)left/total;\n"
                "}\n\n";

        vector <Attribute> var[10];
        vector <pair<int, int>> ranges[10];

        var[0].push_back(Attribute("lineitem", "l_discount"));
        ranges[0].push_back(make_pair(5,15));
        var[0].push_back(Attribute("lineitem", "l_quantity"));
        ranges[0].push_back(make_pair(0,12));
        var[0].push_back(Attribute("part", "p_partkey"));
        ranges[0].push_back(make_pair(100,200));
        var[0].push_back(Attribute("part", "p_size"));
        ranges[0].push_back(make_pair(1,5));

        var[1].push_back(Attribute("lineitem", "l_quantity"));
        ranges[1].push_back(make_pair(9,21));

        Attribute join_att1 = Attribute("lineitem", "l_partkey");
        Attribute join_att2 = Attribute("part", "p_partkey");

        HistMapNode histMapNode = HistMapNode("Revenue", &root, map_sum, Attribute("lineitem", "l_price"), Attribute("lineitem", "l_discount"));
        HistHistJoinNode histHistJoinNode = HistHistJoinNode("Join", &histMapNode, join_att1, join_att2);
        HistSelectNode histSelectNode = HistSelectNode("Select", &histHistJoinNode, var, 2, ranges);
        HistScanNode histScanNode = HistScanNode("Scan", &histSelectNode);

        root.produce(&attributes);

//        HistSelectNode histSelectNode = HistSelectNode(&mapNode, variables, range);
//        HistScanNode histScanNode = HistScanNode(&histSelectNode);
//
//        histSelectNode.set_child(&histScanNode);
//        mapNode.set_child(&histSelectNode);
//        root.set_child(&mapNode);
//
//        root.produce(&attributes);
    }

    catch (const char* massage){
        cerr << massage << endl;
    }
}
