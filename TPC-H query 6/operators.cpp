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



//        vector<string> int_variables;
//        int_variables.push_back("shipdate");
//        int_variables.push_back("discount");
//        int_variables.push_back("quantity");
//        vector<pair<int, int>> range;
//        range.push_back(make_pair(date_to_days("1994-01-01"), date_to_days("1995-01-01")));
//        range.push_back(make_pair(5, 7));
//        range.push_back(make_pair(0, 24));

        Node root = Node("Root");

        //on real data:
        cout<<"==============================================\n============ Code For Real Table ======"
              "=======\n==============================================\n\n";
//        MapNode mapNode = MapNode(&root, "(price * discount)", "revenue");

//        DataSelectNode dataSelectNode = DataSelectNode(&mapNode, int_variables, range);
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
        cout << "\ndouble string_limit(int bars[], bool valid[]){\n"
                "\tint total = 0;\n"
                "\tint left = 0;\n"
                "\tfor (int i = 0; i < bucket_count; ++i) {\n"
                "\t\ttotal += bar[i];\n"
                "\t\tif(!valid[i])\n"
                "\t\t\tbar[i] = 0;\n"
                "\t\tleft += bar[i];\n"
                "\t\t}\n"
                "\t}\n"
                "\treturn (double)left/total;\n"
                "}\n\n";

        vector <Attribute> var[10];
        vector <pair<int, int>> ranges[10];
        vector <Attribute> strings[10];
        vector<vector<string>> valid_strings[10];

        vector<string> tmp[3][4];

        Attribute p_container_att = Attribute("part", "p_container", data_att, att_string);
        Attribute p_brand_att = Attribute("part", "p_brand", data_att, att_string);
        Attribute p_partkey_att = Attribute("part", "p_partkey", data_att, att_int);
        Attribute p_size_att = Attribute("part", "p_size", data_att, att_int);
        Attribute l_shipmode_att = Attribute("lineitem", "l_shipmode", data_att, att_string);
        Attribute l_shipinstruct_att = Attribute("lineitem", "l_shipinstruct", data_att, att_string);
        Attribute l_quantity_att = Attribute("lineitem", "l_quantity", data_att, att_int);
        Attribute l_discount_att = Attribute("lineitem", "l_discount", data_att, att_int);
        Attribute l_price_att = Attribute("lineitem", "l_price", data_att, att_int);
        Attribute l_partkey_att = Attribute("lineitem", "l_partkey", data_att, att_int);

        set<Attribute> attributes;

//        attributes.insert( attribute("lineitem", "discount"));

        attributes.insert(p_container_att);
        attributes.insert(p_brand_att);
        attributes.insert(p_partkey_att);
        attributes.insert(p_size_att);
        attributes.insert(l_shipmode_att);
        attributes.insert(l_shipinstruct_att);
        attributes.insert(l_quantity_att);
        attributes.insert(l_discount_att);
        attributes.insert(l_price_att);
        attributes.insert(l_partkey_att);

        //////////////////////////////////////////////////////////////////////////////
        strings[0].push_back(p_container_att);
        tmp[0][0].push_back("SM_CASE");
        tmp[0][0].push_back("SM_BOX");
        tmp[0][0].push_back("SM_PACK");
        tmp[0][0].push_back("SM_PKG");
        valid_strings[0].push_back(tmp[0][0]);

        strings[0].push_back(p_brand_att);
        tmp[0][1].push_back("Brand#12");
        valid_strings[0].push_back(tmp[0][1]);

        strings[0].push_back(l_shipmode_att);
        tmp[0][2].push_back("AIR");
        tmp[0][2].push_back("AIR_REG");
        valid_strings[0].push_back(tmp[0][2]);

        strings[0].push_back(l_shipinstruct_att);
        tmp[0][3].push_back("DELIVER_IN_PERSON");
        valid_strings[0].push_back(tmp[0][3]);

        var[0].push_back(l_quantity_att);
        ranges[0].push_back(make_pair(0,12));
        var[0].push_back(p_size_att);
        ranges[0].push_back(make_pair(1,5));
        //////////////////////////////////////////////////////////////////////////////
        strings[1].push_back(p_container_att);
        tmp[1][0].push_back("MED_BAG");
        tmp[1][0].push_back("MED_BOX");
        tmp[1][0].push_back("MED_PACK");
        tmp[1][0].push_back("MED_PKG");
        valid_strings[1].push_back(tmp[1][0]);

        strings[1].push_back(p_brand_att);
        tmp[1][1].push_back("Brand#23");
        valid_strings[1].push_back(tmp[1][1]);

        strings[1].push_back(l_shipmode_att);
        tmp[1][2].push_back("AIR");
        tmp[1][2].push_back("AIR_REG");
        valid_strings[1].push_back(tmp[1][2]);

        strings[1].push_back(l_shipinstruct_att);
        tmp[1][3].push_back("DELIVER_IN_PERSON");
        valid_strings[1].push_back(tmp[1][3]);

        var[1].push_back(l_quantity_att);
        ranges[1].push_back(make_pair(9,21));
        var[1].push_back(p_size_att);
        ranges[1].push_back(make_pair(1,10));
        //////////////////////////////////////////////////////////////////////////////
        strings[2].push_back(p_container_att);
        tmp[2][0].push_back("LG_CASE");
        tmp[2][0].push_back("LG_BOX");
        tmp[2][0].push_back("LG_PACK");
        tmp[2][0].push_back("LG_PKG");
        valid_strings[2].push_back(tmp[2][0]);

        strings[2].push_back(p_brand_att);
        tmp[2][1].push_back("Brand#34");
        valid_strings[2].push_back(tmp[2][1]);

        strings[2].push_back(l_shipmode_att);
        tmp[2][2].push_back("AIR");
        tmp[2][2].push_back("AIR_REG");
        valid_strings[2].push_back(tmp[2][2]);

        strings[2].push_back(l_shipinstruct_att);
        tmp[2][3].push_back("DELIVER_IN_PERSON");
        valid_strings[2].push_back(tmp[2][3]);

        var[2].push_back(l_quantity_att);
        ranges[2].push_back(make_pair(19,31));
        var[2].push_back(p_size_att);
        ranges[2].push_back(make_pair(1,15));
        //////////////////////////////////////////////////////////////////////////////



        Attribute join_att1 = l_partkey_att;
        Attribute join_att2 = p_partkey_att;

        MapNode histMapNode = MapNode("Revenue", &root, map_sum, data_att, l_price_att, l_discount_att);
        JoinNode histHistJoinNode = JoinNode("Join", &histMapNode, join_att1, join_att2);
        SelectNode histSelectNode = SelectNode("Select", &histHistJoinNode, var, 3, ranges, 3, strings, valid_strings);
        ScanNode histScanNode = ScanNode("Scan", &histSelectNode);

        root.produce(&attributes);

//        HistSelectNode histSelectNode = HistSelectNode(&mapNode, int_variables, range);
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
