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

void preprocess(string x){
    prep(x);
}

int main(){
    try {
        prepare_curl();
        preprocess("Q6.cpp");
        read_lineitem_from_file("edited_lineitem.tbl", table_ints, table_strings, lineitem_ints, lineitem_strings);

        Node root = Node("Root");
        indent = "\t";

        vector <Attribute> var[10];
        vector <pair<int, int>> ranges[10];
        vector <Attribute> strings[10];
        vector<vector<string>> valid_strings[10];
        vector<string> tmp[3][4];

        Attribute p_container_att = Attribute("part", "p_container", data_att, att_string, p_container_id);
        Attribute p_brand_att = Attribute("part", "p_brand", data_att, att_string, p_brand_id);
        Attribute p_partkey_att = Attribute("part", "p_partkey", data_att, att_int, p_partkey_id);
        Attribute p_size_att = Attribute("part", "p_size", dist_att, "NormalDistribution[4,1]", att_int, p_size_id);
        Attribute l_shipmode_att = Attribute("lineitem", "l_shipmode", data_att, att_string, l_shipmode_id);
        Attribute l_shipinstruct_att = Attribute("lineitem", "l_shipinstruct", data_att, att_string, l_shipinstruct_id);
        Attribute l_quantity_att = Attribute("lineitem", "l_quantity", data_att, att_int, l_quantity_id);
//        Attribute l_discount_att = Attribute("lineitem", "l_discount", dist_att, "UniformDistribution[{0,10}]", att_int, l_discount_id);
        Attribute l_discount_att = Attribute("lineitem", "l_discount", hist_att, att_int, l_discount_id);
        Attribute l_price_att = Attribute("lineitem", "l_price", data_att, att_int, l_price_id);
        Attribute l_partkey_att = Attribute("lineitem", "l_partkey", data_att, att_int, l_partkey_id);
        Attribute l_shipdate_att = Attribute("lineitem", "l_shipdate", data_att, att_int, l_shipdate_id);

        string_id[l_shipmode_id]["AIR"] = 1;
        string_id[l_shipmode_id]["FOB"] = 2;
        string_id[l_shipmode_id]["MAIL"] = 3;
        string_id[l_shipmode_id]["RAIL"] = 4;
        string_id[l_shipmode_id]["REG_AIR"] = 5;
        string_id[l_shipmode_id]["SHIP"] = 6;
        string_id[l_shipmode_id]["TRUCK"] = 7;

        string_id[l_shipinstruct_id]["COLLECT_COD"] = 1;
        string_id[l_shipinstruct_id]["DELIVER_IN_PERSON"] = 2;
        string_id[l_shipinstruct_id]["NONE"] = 3;
        string_id[l_shipinstruct_id]["TAKE_BACK_RETURN"] = 4;

        string_id[p_container_id]["JUMBO_BAG"] = 1;
        string_id[p_container_id]["JUMBO_BOX"] = 2;
        string_id[p_container_id]["JUMBO_CAN"] = 3;
        string_id[p_container_id]["JUMBO_CASE"] = 4;
        string_id[p_container_id]["JUMBO_DRUM"] = 5;
        string_id[p_container_id]["JUMBO_JAR"] = 6;
        string_id[p_container_id]["JUMBO_PACK"] = 7;
        string_id[p_container_id]["JUMBO_PKG"] = 8;
        string_id[p_container_id]["LG_BAG"] = 9;
        string_id[p_container_id]["LG_BOX"] = 10;
        string_id[p_container_id]["LG_CAN"] = 11;
        string_id[p_container_id]["LG_CASE"] = 12;
        string_id[p_container_id]["LG_DRUM"] = 13;
        string_id[p_container_id]["LG_JAR"] = 14;
        string_id[p_container_id]["LG_PACK"] = 15;
        string_id[p_container_id]["LG_PKG"] = 16;
        string_id[p_container_id]["MED_BAG"] = 17;
        string_id[p_container_id]["MED_BOX"] = 18;
        string_id[p_container_id]["MED_CAN"] = 19;
        string_id[p_container_id]["MED_CASE"] = 20;
        string_id[p_container_id]["MED_DRUM"] = 21;
        string_id[p_container_id]["MED_JAR"] = 22;
        string_id[p_container_id]["MED_PACK"] = 23;
        string_id[p_container_id]["MED_PKG"] = 24;
        string_id[p_container_id]["SM_BAG"] = 25;
        string_id[p_container_id]["SM_BOX"] = 26;
        string_id[p_container_id]["SM_CAN"] = 27;
        string_id[p_container_id]["SM_CASE"] = 28;
        string_id[p_container_id]["SM_DRUM"] = 29;
        string_id[p_container_id]["SM_JAR"] = 30;
        string_id[p_container_id]["SM_PACK"] = 31;
        string_id[p_container_id]["SM_PKG"] = 32;
        string_id[p_container_id]["WRAP_BAG"] = 33;
        string_id[p_container_id]["WRAP_BOX"] = 34;
        string_id[p_container_id]["WRAP_CAN"] = 35;
        string_id[p_container_id]["WRAP_CASE"] = 36;
        string_id[p_container_id]["WRAP_DRUM"] = 37;
        string_id[p_container_id]["WRAP_JAR"] = 38;
        string_id[p_container_id]["WRAP_PACK"] = 39;
        string_id[p_container_id]["WRAP_PKG"] = 40;

        string_id[p_brand_id]["Brand#11"] = 1;
        string_id[p_brand_id]["Brand#12"] = 2;
        string_id[p_brand_id]["Brand#13"] = 3;
        string_id[p_brand_id]["Brand#14"] = 4;
        string_id[p_brand_id]["Brand#15"] = 5;
        string_id[p_brand_id]["Brand#21"] = 6;
        string_id[p_brand_id]["Brand#22"] = 7;
        string_id[p_brand_id]["Brand#23"] = 8;
        string_id[p_brand_id]["Brand#24"] = 9;
        string_id[p_brand_id]["Brand#25"] = 10;
        string_id[p_brand_id]["Brand#31"] = 11;
        string_id[p_brand_id]["Brand#32"] = 12;
        string_id[p_brand_id]["Brand#33"] = 13;
        string_id[p_brand_id]["Brand#34"] = 14;
        string_id[p_brand_id]["Brand#35"] = 15;
        string_id[p_brand_id]["Brand#41"] = 16;
        string_id[p_brand_id]["Brand#42"] = 17;
        string_id[p_brand_id]["Brand#43"] = 18;
        string_id[p_brand_id]["Brand#44"] = 19;
        string_id[p_brand_id]["Brand#45"] = 20;
        string_id[p_brand_id]["Brand#51"] = 21;
        string_id[p_brand_id]["Brand#52"] = 22;
        string_id[p_brand_id]["Brand#53"] = 23;
        string_id[p_brand_id]["Brand#54"] = 24;
        string_id[p_brand_id]["Brand#55"] = 25;

        set<Attribute> attributes;

//        attributes.insert(p_container_att);
//        attributes.insert(p_brand_att);
//        attributes.insert(p_partkey_att);
//        attributes.insert(p_size_att);
        attributes.insert(l_shipmode_att);
        attributes.insert(l_shipinstruct_att);
        attributes.insert(l_quantity_att);
        attributes.insert(l_discount_att);
        attributes.insert(l_price_att);
        attributes.insert(l_partkey_att);
        attributes.insert(l_shipdate_att);

        set<Table> tables;
        tables.insert(Table("lineitem"));
//        tables.insert(Table("part"));

        //////////////////////////////////////////////////////////////////////////////

        var[0].push_back(l_quantity_att);
        ranges[0].push_back(make_pair(1,24));
        var[0].push_back(l_discount_att);
        ranges[0].push_back(make_pair(6, 8));
        var[0].push_back(l_shipdate_att);
        ranges[0].push_back(make_pair(date_to_days("1994-01-01"), date_to_days("1995-01-01")));


        ////////////////////////////////////////////////////////////////////////////

        vector<Attribute*> vect{&l_price_att, &l_discount_att};

        OperandNode opd2(&l_discount_att);
        OperandNode opd3(&l_price_att);
        OperatorNode opr2(&opd3, &opd2, false, "mult", false);

        AggregateNode aggregateNode = AggregateNode("sum_price", &root, agg_sum, data_att, &opr2);
        SelectNode selectNode = SelectNode("Select", &aggregateNode, var, 1, ranges, strings, 0, valid_strings);
        ScanNode scanNode1 = ScanNode("ScanL", &selectNode);

        fprintf(pfile, "\n#include \"database_preparation.h\"\n"
                       "\n"
                       "using namespace std;\n\n");

//        wolframAggregateNode.prep();
        aggregateNode.prep();
        string s = "\n";
        s += "\tint lineitem_ia[] = {" + l_discount_att.name + "_id, " + l_price_att.name + "_id, " + l_shipdate_att.name + "_id, " +
                l_quantity_att.name + "_id};\n";
        s += "\tint lineitem_sa[] = {" + l_shipinstruct_att.name + "_id, " + l_shipmode_att.name + "_id};";

        fprintf(pfile,
                "\nint main(){\n"
                "\n%s\n"
                "    read_lineitems_from_file(\"edited_lineitem.tbl\", lineitem_ia, 4, lineitem_sa, 2);\n"
                "    read_histogram_defaults_from_file(\"histograms.txt\");\n",
        s.c_str());
        selectNode.prep();
        scanNode1.prep();

        set<string> x;
        root.produce(&attributes, tables, &x);
//        fprintf(pfile,
//                "\tdouble sum = 0;\n"
//                       "\tcout << endl;\n"
//                       "\tfor (int i = 1 ; i < 50 ; i ++ ){\n"
//                       "\t\tif(search(groupby_hash_id, i) != NULL){\n"
//                       "\t\t\tcout << i << \" \" << sum_disc_price[search(groupby_hash_id, i) -> data] << endl;\n"
//                       "\t\t\t\tsum += sum_disc_price[search(groupby_hash_id, i) -> data];\n"
//                       "\t\t}\n"
//                       "\t}\n"
//                "\tcout << wolfram << endl;");
        fprintf(pfile, "\n}\n");
        cleanup();
    }

    catch (const char* massage){
        cerr << massage << endl;
    }
}
