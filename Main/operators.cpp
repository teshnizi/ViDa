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
        fprintf(pfile,"#include <math.h>\n");
        fprintf(pfile,"#include <stdio.h>\n");
        fprintf(pfile,"#include <string>\n");
        fprintf(pfile,"#include <fstream>\n");
        fprintf(pfile,"#include <sstream>\n");
        fprintf(pfile,"#include <iostream>\n");
        fprintf(pfile,"#include <vector>\n\n");
        fprintf(pfile,"using namespace std;\n\n");
        fprintf(pfile,"const int max_table_num = 2;\n");
        fprintf(pfile,"const int max_table_size = 1000000;\n");
        fprintf(pfile,"const int hash_table_size = 1000000;\n");
        fprintf(pfile,"const int max_hashed_attributes = 3;\n");
        fprintf(pfile,"const int max_attribute_num = 15;\n");
        fprintf(pfile,"const int max_bucket_count = 40;\n");
        fprintf(pfile,"const int BUCKET_COUNT = 40;\n");
        fprintf(pfile,"const int discount_coefficient = 100;\n"
                      "const int tax_coefficient = 100;\n"
                      "const int price_coefficient = 10;\n"
                      "const int retailprice_coefficient = 100;\n");
        fprintf(pfile,"\nenum tables{\n"
                      "    lineitem,\n"
                      "    part\n"
                      "};\n\n");
        fprintf(pfile,"int default_bucket[max_table_num][max_attribute_num][max_bucket_count];\n");
        fprintf(pfile,"int default_minimum[max_table_num][max_attribute_num];\n");
        fprintf(pfile,"int default_maximum[max_table_num][max_attribute_num];\n");
        fprintf(pfile,"int default_bucket_width[max_table_num][max_attribute_num];\n");
        fprintf(pfile,"int default_table_size[max_table_num];\n");
        fprintf(pfile,"int table_size[max_table_num];\n");
        fprintf(pfile,"int int_table[max_table_num][max_attribute_num][max_table_size];\n");
        fprintf(pfile,"string string_table[max_table_num][max_attribute_num][max_table_size];\n");



        fprintf(pfile, "\nenum l_table{\n"
                       "    l_orderkey_id = 0,\n"
                       "    l_quantity_id ,\n"
                       "    l_price_id ,\n"
                       "    l_discount_id ,\n"
                       "    l_shipdate_id ,\n"
                       "    l_partkey_id ,\n"
                       "    l_suppkey_id ,\n"
                       "    l_linenumber_id ,\n"
                       "    l_tax_id ,\n"
                       "    l_returnflag_id ,\n"
                       "    l_linestatus_id ,\n"
                       "    l_commitdate_id ,\n"
                       "    l_receiptdate_id ,\n"
                       "    l_shipinstruct_id ,\n"
                       "    l_shipmode_id,\n"
                       "};\n"
                       "\n"
                       "enum p_table{\n"
                       "    p_partkey_id = 0,\n"
                       "    p_name_id,\n"
                       "    p_mfgr_id,\n"
                       "    p_brand_id,\n"
                       "    p_type_id,\n"
                       "    p_size_id,\n"
                       "    p_container_id,\n"
                       "    p_retailprice_id,\n"
                       "    p_comment_id,\n"
                       "};\n");
        fprintf(pfile,"\n\n"
                      "void read_lineitems_from_file(string name,\n"
                      "int *int_attributes, int num_of_int_attributes, int *string_attributes, int num_of_string_attributes){\n"
                      "    ifstream fin;\n"
                      "    fin.open(name);\n"
                      "    string line;\n"
                      "    int int_tmp[20];\n"
                      "    float float_tmp[20];\n"
                      "    char char_tmp[20];\n"
                      "    string string_tmp[20];\n"
                      "    int iterator = 0;"
                      "\n"
                      "while (getline(fin,line,'\\n')) {\n"
                      "        istringstream iss(line);\n"
                      "        iss >> int_tmp[l_orderkey_id] >> int_tmp[l_partkey_id] >> int_tmp[l_suppkey_id] >> int_tmp[l_linenumber_id] >> int_tmp[l_quantity_id] >> float_tmp[l_price_id] >>\n"
                      "            float_tmp[l_discount_id] >> float_tmp[l_tax_id] >> char_tmp[l_returnflag_id] >> char_tmp[l_linestatus_id] >> int_tmp[l_shipdate_id] >>\n"
                      "            int_tmp[l_commitdate_id] >> int_tmp[l_receiptdate_id] >> string_tmp[l_shipinstruct_id] >> string_tmp[l_shipmode_id];\n"
                      "\n"
                      "        int_tmp[l_discount_id] = discount_coefficient * float_tmp[l_discount_id];\n"
                      "        int_tmp[l_tax_id] = tax_coefficient * float_tmp[l_tax_id];\n"
                      "        int_tmp[l_price_id] = price_coefficient * float_tmp[l_price_id];\n"
                      "        int_tmp[l_returnflag_id] = char_tmp[l_returnflag_id];\n"
                      "        int_tmp[l_linestatus_id] = char_tmp[l_linestatus_id];\n"
                      "\n"
                      "        for (int i = 0; i < num_of_int_attributes; ++i)\n"
                      "            int_table[lineitem][int_attributes[i]][iterator] = int_tmp[int_attributes[i]];\n"
                      "            // table_ints[int_attributes[i]].push_back(int_tmp[int_attributes[i]]);\n"
                      "        for (int i = 0; i < num_of_string_attributes; ++i)\n"
                      "            string_table[lineitem][string_attributes[i]][iterator] = string_tmp[string_attributes[i]];\n"
                      "            // table_strings[string_attributes[i]].push_back(string_tmp[string_attributes[i]]);\n"
                      "        iterator++;\n"
                      "    }\ntable_size[lineitem] = iterator;\n}\n\n\n");
        fprintf(pfile,"\nvoid read_part_from_file(string name,\n"
                      "                         int *int_attributes, int num_of_int_attributes, int *string_attributes, int num_of_string_attributes){\n"
                      "    ifstream fin;\n"
                      "    fin.open(name);\n"
                      "    string line;\n"
                      "    int int_tmp[20];\n"
                      "    float float_tmp[20];\n"
                      "    char char_tmp[20];\n"
                      "    string string_tmp[20];\n"
                      "    int iterator = 0;\n"
                      "    while (getline(fin,line,'\\n')) {\n"
                      "        istringstream iss(line);\n"
                      "        iss >> int_tmp[p_partkey_id] >> string_tmp[p_name_id] >> string_tmp[p_mfgr_id] >> string_tmp[p_brand_id] >> string_tmp[p_type_id] >> int_tmp[p_size_id] >>\n"
                      "            string_tmp[p_container_id] >> float_tmp[p_retailprice_id] >> string_tmp[p_comment_id];\n"
                      "        int_tmp[p_retailprice_id] = retailprice_coefficient * float_tmp[p_retailprice_id];\n"
                      "\n"
                      "        for (int i = 0; i < num_of_int_attributes; ++i) {\n"
                      "            int_table[part][int_attributes[i]][iterator] = int_tmp[int_attributes[i]];\n"
                      "        }\n"
                      "\n"
                      "        for (int i = 0; i < num_of_string_attributes; ++i) {\n"
                      "            string_table[part][string_attributes[i]][iterator] = string_tmp[string_attributes[i]];\n"
                      "        }\n"
                      "        iterator++;\n"
                      "    }\n"
                      "    table_size[part] = iterator;\n"
                      "}\n\n\n");

        fprintf(pfile, "\ndouble limit(int bucket[], int minimum, int maximum, int l, int h){\n"
                "\tint bucket_size = ceil((float)(maximum + 1 - minimum)/(BUCKET_COUNT));\n"
                "\tint total = 0;\n"
                "\tint left = 0;\n"
                "\tfor (int i = 0; i < BUCKET_COUNT; ++i) {\n"
                "        int lo = minimum + bucket_size * i;\n"
                "\t\tint hi = lo + bucket_size;\n"
                "\t\ttotal += bucket[i];\n"
                "\t\tif (lo >= h || hi < l)\n"
                "\t\t\tbucket[i] = 0;\n"
                "        else if ( l <= lo && hi < h)\n"
                "            left += bucket[i];\n"
                "        else{\n"
                "\t\t\tleft += bucket[i];\n"
                "            bucket[i] = bucket[i] * ((lo < l) * (hi - l) + (lo >= l) * (h - lo))/(hi - lo);\n"
                "\t\t\tleft -= bucket[i];\n"
                "        }\n"
                "\t}\n"
                "\treturn (double)left/total;\n"
                "}\n\n");
        fprintf(pfile, "\ndouble string_limit(int bucket[], bool valid[]){\n"
                "\tint total = 0;\n"
                "\tint left = 0;\n"
                "\tfor (int i = 0; i < BUCKET_COUNT; ++i) {\n"
                "\t\ttotal += bucket[i];\n"
                "\t\tif(!valid[i])\n"
                "\t\t\tbucket[i] = 0;\n"
                "\t\tleft += bucket[i];\n"
                "\t}\n"
                "\treturn (double)left/total;\n"
                "}\n\n");

        fprintf(pfile, "\n"
                       "struct DataItem{\n"
                       "    int data;\n"
                       "    int key;\n"
                       "};\n"
                       "\n"
                       "typedef struct DataItem DataItem;\n"
                       "\n"
                       "DataItem* hashArray[max_hashed_attributes][hash_table_size * 2];\n"
                       "struct DataItem* dummyItem;\n"
                       "\n"
                       "int hashCode(int key) {\n"
                       "    return key %% hash_table_size;\n"
                       "}\n"
                       "\n"
                       "struct DataItem *search(int id, int key) {\n"
                       "    //get the hash\n"
                       "    int hashIndex = hashCode(key);\n"
                       "\n"
                       "    //move in array until an empty\n"
                       "    while(hashArray[id][hashIndex] != NULL) {\n"
                       "\n"
                       "        if(hashArray[id][hashIndex]->key == key)\n"
                       "            return hashArray[id][hashIndex];\n"
                       "\n"
                       "        //go to next cell\n"
                       "        ++hashIndex;\n"
                       "\n"
                       "        //wrap around the table\n"
                       "        hashIndex %%= hash_table_size;\n"
                       "    }\n"
                       "    return NULL;\n"
                       "}\n"
                       "\n"
                       "void insert(int id, int key,int data) {\n"
                       "    struct DataItem *item = (struct DataItem*) malloc(sizeof(struct DataItem));\n"
                       "    item->data = data;\n"
                       "    item->key = key;\n"
                       "\n"
                       "    //get the hash\n"
                       "    int hashIndex = hashCode(key);\n"
                       "\n"
                       "    //move in array until an empty or deleted cell\n"
                       "    while(hashArray[id][hashIndex] != NULL && hashArray[id][hashIndex]->key != -1) {\n"
                       "        //go to next cell\n"
                       "        ++hashIndex;\n"
                       "\n"
                       "        //wrap around the table\n"
                       "        hashIndex %%= hash_table_size;\n"
                       "    }\n"
                       "    hashArray[id][hashIndex] = item;\n"
                       "}\n"
                       "\n"
                       "struct DataItem* remove(int id, struct DataItem* item) {\n"
                       "    int key = item->key;\n"
                       "\n"
                       "    //get the hash\n"
                       "    int hashIndex = hashCode(key);\n"
                       "\n"
                       "    //move in array until an empty\n"
                       "    while (hashArray[hashIndex] != NULL) {\n"
                       "\n"
                       "        if (hashArray[id][hashIndex]->key == key) {\n"
                       "            struct DataItem *temp = hashArray[id][hashIndex];\n"
                       "\n"
                       "            //assign a dummy item at deleted position\n"
                       "            hashArray[id][hashIndex] = dummyItem;\n"
                       "            return temp;\n"
                       "        }\n"
                       "\n"
                       "        //go to next cell\n"
                       "        ++hashIndex;\n"
                       "\n"
                       "        //wrap around the table\n"
                       "        hashIndex %%= hash_table_size;\n"
                       "    }\n"
                       "\n"
                       "    return NULL;\n"
                       "}\n");

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

        fprintf(pfile, "int main(){\n"
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
                       "    }\n");
        root.produce(&attributes);
//        fprintf(pfile, "\n}\n");
    }

    catch (const char* massage){
        cerr << massage << endl;
    }
}