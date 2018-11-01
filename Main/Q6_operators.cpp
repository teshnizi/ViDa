//
// Created by teshnizi on 24/08/18.
//

#include "Q19_operators.h"

using namespace std;

vector<int> table_ints[ENUM_COUNT];
vector<string> table_strings[ENUM_COUNT];

vector<int> lineitem_ints;
vector<int> lineitem_strings;

Histogram hist_ints[ENUM_COUNT];

int lineitem_table_size;

pair<int, int> condition_ranges[ENUM_COUNT];

void preprocess(){
    lineitem_ints.push_back        (l_partkey_id);
    lineitem_ints.push_back        (l_quantity_id);
    lineitem_ints.push_back        (l_price_id);
    lineitem_ints.push_back        (l_discount_id);
    lineitem_ints.push_back        (l_tax_id);
    lineitem_strings.push_back     (l_shipmode_id);
    lineitem_strings.push_back     (l_shipinstruct_id);
}


int main(){
    try {
        preprocess();
        prep("HELLO.cpp");
        read_lineitem_from_file("edited_lineitem.tbl", table_ints, table_strings, lineitem_ints, lineitem_strings);
        lineitem_table_size = table_ints[lineitem_ints[0]].size();
//        for (int i = 0; i < ENUM_COUNT; ++i) {
//            if (table_ints[i].size() > 0)
//                hist_ints[i] = Histogram(&table_ints[i], 40);
//        }

        Node root = Node("Root");
        indent = "\t";

//        fprintf(pfile,"==============================================\n============ Code For Histograms ======="
//              "======\n==============================================\n\n");

        vector <Attribute> var[10];
        vector <pair<int, int>> ranges[10];
        vector <Attribute> strings[10];
        vector<vector<string>> valid_strings[10];

        vector<string> tmp[3][4];

        Attribute l_quantity_att = Attribute("lineitem", "l_quantity", data_att, att_int, l_quantity_id);
        Attribute l_discount_att = Attribute("lineitem", "l_discount", data_att, att_int, l_discount_id);
        Attribute l_price_att = Attribute("lineitem", "l_price", hist_att, att_int, l_price_id);
        Attribute l_shipdate_att = Attribute("lineitem", "l_shipdate", data_att, att_int, l_shipdate_id);


        set<Attribute> attributes;

        attributes.insert(l_quantity_att);
        attributes.insert(l_discount_att);
        attributes.insert(l_price_att);
        attributes.insert(l_shipdate_att);

        set<Table> tables;
        tables.insert(Table("lineitem"));

        //////////////////////////////////////////////////////////////////////////////
//        strings[0].push_back(p_container_att);
//        tmp[0][0].push_back("SM_CASE");
//        tmp[0][0].push_back("SM_BOX");
//        tmp[0][0].push_back("SM_PACK");
//        tmp[0][0].push_back("SM_PKG");
//        valid_strings[0].push_back(tmp[0][0]);
//
//        strings[0].push_back(p_brand_att);
//        tmp[0][1].push_back("Brand#12");
//        valid_strings[0].push_back(tmp[0][1]);
//
//        strings[0].push_back(l_shipmode_att);
//        tmp[0][2].push_back("AIR");
//        tmp[0][2].push_back("REG_AIR");
//        valid_strings[0].push_back(tmp[0][2]);
//
//        strings[0].push_back(l_shipinstruct_att);
//        tmp[0][3].push_back("DELIVER_IN_PERSON");
//        valid_strings[0].push_back(tmp[0][3]);

        var[0].push_back(l_quantity_att);
        ranges[0].push_back(make_pair(1,24));
        var[0].push_back(l_discount_att);
        ranges[0].push_back(make_pair(5,8));
        var[0].push_back(l_shipdate_att);
        ranges[0].push_back(make_pair(date_to_days("1994-01-01"),date_to_days("1995-01-01")));

        OperandNode opd1(&l_price_att);
        OperandNode opd2(&l_discount_att);
        OperatorNode opr(&opd1, &opd2, false, "mult", false);

        //////////////////////////////////////////////////////////////////////////////

        AggregateNode aggregateNode1 = AggregateNode("Revenue", &root, agg_sum, data_att, &opr);

        SelectNode selectNode = SelectNode("Select", &aggregateNode1, var, 1, ranges, strings, 0, valid_strings);

        ScanNode scanNode1 = ScanNode("ScanL", &selectNode);

        fprintf(pfile, "\n#include \"database_preparation.h\"\n"
                       "\n"
                       "using namespace std;\n\n");

        aggregateNode1.prep();

        fprintf(pfile,
                "\nint main(){\n"
                "\n"
                "    int lineitem_ia[] = {l_discount_id, l_partkey_id, l_price_id, l_quantity_id, l_shipdate_id};\n"
                "    int lineitem_sa[] = {l_shipinstruct_id, l_shipmode_id};\n"
                "    read_lineitems_from_file(\"edited_lineitem.tbl\", lineitem_ia, 5, lineitem_sa, 2);\n"
                "    read_histogram_defaults_from_file(\"histograms.txt\");\n\n\n");

        scanNode1.prep();

        set<string> x;
        root.produce(&attributes, tables, &x);
        fprintf(pfile, "\tcout << Revenue << endl;");
        fprintf(pfile, "\n}\n");
    }

    catch (const char* massage){
        cerr << massage << endl;
    }
}

// javaba kheili fargh daran. hist 1.7e9 data 3e8. bayad ino check konam ke be khatere expressione fargh ya na.
// l_quantity tooye shart ham oomade va bayad bebinam tafavote rad kardane shart beyn data o hist cheghadre.