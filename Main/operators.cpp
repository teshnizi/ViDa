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
        prep("HELLO.cpp");
        read_lineitem_from_file("edited_lineitem.tbl", table_ints, table_strings, lineitem_ints, lineitem_strings);
        lineitem_table_size = table_ints[lineitem_ints[0]].size();
        for (int i = 0; i < ENUM_COUNT; ++i) {
            if (table_ints[i].size() > 0)
                hist_ints[i] = Histogram(&table_ints[i], 40);
        }

        Node root = Node("Root");
        indent = "\t";

//        fprintf(pfile,"==============================================\n============ Code For Histograms ======="
//              "======\n==============================================\n\n");

        vector <Attribute> var[10];
        vector <pair<int, int>> ranges[10];
        vector <Attribute> strings[10];
        vector<vector<string>> valid_strings[10];

        vector<string> tmp[3][4];

        Attribute p_container_att = Attribute("part", "p_container", data_att, att_string, p_container);
        Attribute p_brand_att = Attribute("part", "p_brand", data_att, att_string, p_brand);
        Attribute p_partkey_att = Attribute("part", "p_partkey", data_att, att_int, p_partkey);
        Attribute p_size_att = Attribute("part", "p_size", data_att, att_int, p_size);
        Attribute l_shipmode_att = Attribute("lineitem", "l_shipmode", hist_att, att_string, l_shipmode);
        Attribute l_shipinstruct_att = Attribute("lineitem", "l_shipinstruct", data_att, att_string, l_shipinstruct);
        Attribute l_quantity_att = Attribute("lineitem", "l_quantity", data_att, att_int, l_quantity);
        Attribute l_discount_att = Attribute("lineitem", "l_discount", data_att, att_int, l_discount);
        Attribute l_price_att = Attribute("lineitem", "l_price", data_att, att_int, l_price);
        Attribute l_partkey_att = Attribute("lineitem", "l_partkey", data_att, att_int, l_partkey);

        string_id[l_shipmode]["AIR"] = 1;
        string_id[l_shipmode]["FOB"] = 2;
        string_id[l_shipmode]["MAIL"] = 3;
        string_id[l_shipmode]["RAIL"] = 4;
        string_id[l_shipmode]["REG_AIR"] = 5;
        string_id[l_shipmode]["SHIP"] = 6;
        string_id[l_shipmode]["TRUCK"] = 7;

        string_id[l_shipinstruct]["DELIVER_IN_PERSON"] = 1;

        string_id[p_container]["SM_CASE"] = 1;
        string_id[p_container]["SM_BOX"] = 2;
        string_id[p_container]["SM_PACK"] = 3;
        string_id[p_container]["SM_PKG"] = 4;
        string_id[p_container]["MED_BAG"] = 5;
        string_id[p_container]["MED_BOX"] = 6;
        string_id[p_container]["MED_PKG"] = 7;
        string_id[p_container]["LG_CASE"] = 8;
        string_id[p_container]["LG_BOX"] = 9;
        string_id[p_container]["LG_PACK"] = 10;
        string_id[p_container]["LG_PKG"] = 11;

        string_id[p_brand]["Brand#12"] = 1;
        string_id[p_brand]["Brand#23"] = 2;
        string_id[p_brand]["Brand#34"] = 3;

        set<Attribute> attributes;

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
        tmp[0][2].push_back("REG_AIR");
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
        tmp[1][0].push_back("MED_PKG");
        valid_strings[1].push_back(tmp[1][0]);

        strings[1].push_back(p_brand_att);
        tmp[1][1].push_back("Brand#23");
        valid_strings[1].push_back(tmp[1][1]);

        strings[1].push_back(l_shipmode_att);
        tmp[1][2].push_back("AIR");
        tmp[1][2].push_back("REG_AIR");
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
        tmp[2][2].push_back("REG_AIR");
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

        AggregateNode aggregateNode = AggregateNode("Revenue", &root, map_mult, data_att, l_price_att, l_discount_att);
        SelectNode selectNode = SelectNode("Select", &aggregateNode, var, 3, ranges, 3, strings, valid_strings);
        JoinNode joinNode = JoinNode("Join", &selectNode, join_att1, join_att2);
        ScanNode scanNode1 = ScanNode("ScanL", &joinNode);
        HashScanNode scanNode2 = HashScanNode("ScanP", &joinNode, &l_partkey_att, &p_partkey_att);

        joinNode.setLeftChild(&scanNode1);
        tmp[1][0].push_back("MED_PACK");
        joinNode.setRightChild(&scanNode2);

        fprintf(pfile, "\n#include \"database_preparation.h\"\n"
                       "\n"
                       "using namespace std;\n\n"
                       "int main(){\n"
                       "\n"
                       "    int lineitem_ia[] = {l_discount_id, l_partkey_id, l_price_id, l_quantity_id};\n"
                       "    int lineitem_sa[] = {l_shipinstruct_id, l_shipmode_id};\n"
                       "    read_lineitems_from_file(\"edited_lineitem.tbl\", lineitem_ia, 4, lineitem_sa, 2);\n"
                       "\n"
                       "    int part_ia[] = {p_partkey_id, p_size_id};\n"
                       "    int part_sa[] = {p_container_id, p_brand_id};\n"
                       "    read_part_from_file(\"edited_part.tbl\", part_ia, 2, part_sa, 2);\n"
                       "    for (int i = 0; i < table_size[part]; ++i) {\n"
                       "        insert(p_partkey_id, int_table[part][p_partkey_id][i], i);\n"
                       "    }\n"
                       "\n"
                       "    read_histogram_defaults_from_file(\"histograms.txt\");\n\n");
        root.produce(&attributes);
//        fprintf(pfile, "\n}\n");
    }

    catch (const char* massage){
        cerr << massage << endl;
    }
}