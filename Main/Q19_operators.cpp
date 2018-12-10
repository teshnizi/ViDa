//
// Created by teshnizi on 24/08/18.
//

#include "operators.h"
#include <ginac/ginac.h>

using namespace std;
using namespace GiNaC;

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
        preprocess("HELLO.cpp");
        read_lineitem_from_file("edited_lineitem.tbl", table_ints, table_strings, lineitem_ints, lineitem_strings);

        Node root = Node("Root");
        indent = "\t";

        vector <Attribute> var[10];
        vector <pair<int, int>> ranges[10];
        vector <Attribute> strings[10];
        vector<vector<string>> valid_strings[10];
        vector<string> tmp[3][4];

        Attribute p_container_att = Attribute("part", "pContainer", data_att, att_string, p_container_id);
        Attribute p_brand_att = Attribute("part", "pBrand", data_att, att_string, p_brand_id);
        Attribute p_partkey_att = Attribute("part", "pPartkey", data_att, att_int, p_partkey_id);
        Attribute p_size_att = Attribute("part", "pSize", dist_att, "NormalDistribution[4,1]", att_int, p_size_id);
        Attribute l_shipmode_att = Attribute("lineitem", "lShipmode", data_att, att_string, l_shipmode_id);
        Attribute l_shipinstruct_att = Attribute("lineitem", "lShipinstruct", data_att, att_string, l_shipinstruct_id);
        Attribute l_quantity_att = Attribute("lineitem", "lQuantity", data_att, att_int, l_quantity_id);
        Attribute l_discount_att = Attribute("lineitem", "lDiscount", dist_att, "NormalDistribution[2,2]", att_int, l_discount_id);
        Attribute l_price_att = Attribute("lineitem", "lPrice", data_att, att_int, l_price_id);
        Attribute l_partkey_att = Attribute("lineitem", "lPartkey", data_att, att_int, l_partkey_id);

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

        set<Table> tables;
        tables.insert(Table("lineitem"));
        tables.insert(Table("part"));

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
        ranges[0].push_back(make_pair(1,12));
        var[0].push_back(p_size_att);
        ranges[0].push_back(make_pair(2,5));

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
        ranges[1].push_back(make_pair(10,21));
        var[1].push_back(p_size_att);
        ranges[1].push_back(make_pair(2,10));
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
        ranges[2].push_back(make_pair(20,31));
        var[2].push_back(p_size_att);
        ranges[2].push_back(make_pair(2,15));
        ////////////////////////////////////////////////////////////////////////////

        Attribute join_att1 = l_partkey_att;
        Attribute join_att2 = p_partkey_att;


        OperandNode opd1("10");
        OperandNode opd2(&l_discount_att);
        OperatorNode opr1(&opd1, &opd2, false, "sub", false);
        OperandNode opd3(&l_price_att);
        OperatorNode opr2(&opd3, &opr1, false, "mult", false);
        OperandNode opd4(&l_price_att);

        vector<AggregateNode*> gb_aggs;

        //////////////////////////////////////////////////////////////////////////////

        AggregateNode aggregateNode1 = AggregateNode("sum_disc_price", &root, agg_sum, data_att, &opr2);
        AggregateNode aggregateNode2 = AggregateNode("sum_price", &root, agg_sum, data_att, &opd4);

        gb_aggs.push_back(&aggregateNode1);
        gb_aggs.push_back(&aggregateNode2);

        GroupNode groupNode = GroupNode("groupby", &root, &l_quantity_att, &gb_aggs);


        aggregateNode1.child = &groupNode;
        aggregateNode2.child = &groupNode;


        vector<Attribute*> vect{&l_price_att, &l_discount_att, &p_size_att};
//        vector<string> symbols{"X"        , "Y"           , "Z"};

        WolframAggregateNode wolframAggregateNode("wolf", &root, "(lPrice ^ 2 ) * ((1 + lDiscount)) + pSize", vect, agg_sum, data_att);


        SelectNode selectNode = SelectNode("Select", &wolframAggregateNode, var, 3, ranges, strings, 3, valid_strings);

        JoinNode joinNode = JoinNode("Join", &selectNode, join_att1, join_att2);

        ScanNode scanNode1 = ScanNode("ScanL", &joinNode);

//        ScanNode scanNode2 = ScanNode("ScanP", &selectNode);
        HashScanNode scanNode2 = HashScanNode("ScanP", &joinNode, &l_partkey_att, &p_partkey_att);


        joinNode.setLeftChild(&scanNode1);
        joinNode.setRightChild(&scanNode2);

        fprintf(pfile, "\n#include \"database_preparation.h\"\n"
                       "\n"
                       "using namespace std;\n\n");

        groupNode.prep();
        wolframAggregateNode.prep();
        aggregateNode1.prep();
        aggregateNode2.prep();

        fprintf(pfile,
                       "\nint main(){\n"
                       "\n"
                       "    int lineitem_ia[] = {l_discount_id, l_partkey_id, l_price_id, l_quantity_id};\n"
                       "    int lineitem_sa[] = {l_shipinstruct_id, l_shipmode_id};\n"
                       "    read_lineitems_from_file(\"edited_lineitem.tbl\", lineitem_ia, 4, lineitem_sa, 2);\n"
                       "\n"
                       "    int part_ia[] = {p_partkey_id, p_size_id};\n"
                       "    int part_sa[] = {p_container_id, p_brand_id};\n"
                       "    read_part_from_file(\"edited_part.tbl\", part_ia, 2, part_sa, 2);\n"
                       "    read_histogram_defaults_from_file(\"histograms.txt\");\n\n\n");

        selectNode.prep();
        joinNode.prep();
        scanNode1.prep();
        scanNode2.prep();

        set<string> x;
        root.produce(&attributes, tables, &x);
        fprintf(pfile,
//                "\tdouble sum = 0;\n"
//                       "\tcout << endl;\n"
//                       "\tfor (int i = 1 ; i < 50 ; i ++ ){\n"
//                       "\t\tif(search(groupby_hash_id, i) != NULL){\n"
//                       "\t\t\tcout << i << \" \" << sum_disc_price[search(groupby_hash_id, i) -> data] << endl;\n"
//                       "\t\t\t\tsum += sum_disc_price[search(groupby_hash_id, i) -> data];\n"
//                       "\t\t}\n"
//                       "\t}\n"
                       "\tcout << wolf << endl;");
        fprintf(pfile, "\n}\n");
        cleanup();
    }

    catch (const char* massage){
        cerr << massage << endl;
    }
}

// javaba kheili fargh daran. hist 1.7e9 data 3e8. bayad ino check konam ke be khatere expressione fargh ya na.
// l_quantity tooye shart ham oomade va bayad bebinam tafavote rad kardane shart beyn data o hist cheghadre.