#include <iostream>
#include <bitset>
#include <vector>
#include <stdio.h>
#include "tuples2.h"
#include <set>
#include "statistics.h"
#include <map>
#include "expressions.h"

using namespace std;

enum aggregation_type{
    nothing= 0,
    agg_sum,
    agg_mean
};

enum map_type{
    map_sum,
    map_mult
};

const int condition_max_subconditions = 30;

map <string, int> string_id[ENUM_COUNT];

vector <string> extra_ratios;

struct Table{
    Table(string name){
        this->name = name;
    }
    string name;
    bool operator<(const Table& a) const{
        return this->name < a.name;
    }
    bool operator==(const Table& a) const{
        return (this->name == a.name);
    }
};

vector<string> expression_parser(string expression){
    replace(expression.begin(), expression.end(),'*', ' ');
    replace(expression.begin(), expression.end(),'+', ' ');
    replace(expression.begin(), expression.end(),'(', ' ');
    replace(expression.begin(), expression.end(),')', ' ');
    istringstream iss(expression);
    vector<string> v;
    string s;
    while (!iss.eof()){
        iss >> s;
        v.push_back(s);
    } v.pop_back();
    return v;
}

string indent;

class Node;
class ScanNode;
class SelectNode;
class AggregateNode;
class JoinNode;

FILE* pfile;
map <string, int> table_names;

void prep(string name){
    table_names["lineitem"] = 0;
    table_names["part"] = 1;
    pfile = fopen(name.c_str(), "w");
}

///////////////////////////////////////////////////////////////////////////////////////////////////

class Node{
public:
    Node(string name){
        this->name = name;
    }

    virtual void produce(set<Attribute> *a, set<Table> tables, set<string> *fixed_tables) {
        this->child->produce(a, tables, fixed_tables);
//        while(!indent.empty()) {
//            indent = indent.substr(0, indent.size()-1);
//            fprintf(pfile,"%s}\n", indent.c_str());
//        }
    };

    virtual void consume(set<Attribute> *a, set<Table> tables, Node *source, set<string> *fixed_tables) {
        return;
    }
    void set_child(Node* child){
        this->child = child;
    }
    Node* child{};
    Node* parent{};
    string name;
};

class ScanNode : public Node{
public:
    ScanNode(string name, Node* parent) : Node(name){
        this->parent = parent;
        parent->child = this;
    }
    void produce(set<Attribute> *a, set<Table> tables, set<string> *fixed_tables) override;
    void consume(set<Attribute> *a, set<Table> tables, Node *source, set<string> *fixed_tables) override;
};

class HashScanNode : public Node{
public:
    HashScanNode(string name, Node* parent, Attribute* sourceAtt,  Attribute* hashedAtt) : Node(name){
        this->parent = parent;
        parent->child = this;
        this->sourceAtt = sourceAtt;
        this->hashedAtt = hashedAtt;
    }
    void produce(set<Attribute> *a, set<Table> tables, set<string> *fixed_tables) override;
    void consume(set<Attribute> *a, set<Table> tables, Node *source, set<string> *fixed_tables) override;

private:
    Attribute* sourceAtt;
    Attribute* hashedAtt;
};

class SelectNode : public Node{
public:
    SelectNode(string name, Node* parent, vector<Attribute> int_variables[], int int_sub_conditions, vector<pair<int,int>> ranges[],
    int string_sub_conditions, vector<Attribute> string_variables[], vector<vector<string>> valid_strings[]) :
    Node(name) {
        this->int_sub_conditions = int_sub_conditions;
        this->string_sub_conditions = string_sub_conditions;
        for (int i = 0; i < int_sub_conditions; ++i) {
            this->int_variables[i] = int_variables[i];
            this->int_ranges[i] = ranges[i];
        }
        for (int j = 0; j < string_sub_conditions; ++j) {
            this->string_variables[j] = string_variables[j];
            this->valid_strings[j] = valid_strings[j];
        }
        this->parent = parent;
        this->parent->child = this;
    }
    void produce(set<Attribute> *a, set<Table> tables, set<string> *fixed_tables) override;
    void consume(set<Attribute> *a, set<Table> tables, Node *source, set<string> *fixed_tables) override;

private:
    int int_sub_conditions;
    int string_sub_conditions;
    vector<Attribute> int_variables[condition_max_subconditions];
    vector<pair<int, int>> int_ranges[condition_max_subconditions];
    vector<Attribute> string_variables[condition_max_subconditions];
    vector<vector<string>> valid_strings[condition_max_subconditions];
};

class AggregateNode : public Node{
public:
    AggregateNode(string name, Node* parent, int type, int virtuality, Attribute op1, Attribute op2) : Node(name){
        this->parent = parent;
        this->parent->child = this;
        this->type = type;
        this->virtuality = virtuality;
        this->op1 = op1;
        this->op2 = op2;
    }
    void produce(set<Attribute> *a, set<Table> tables, set<string> *fixed_tables) override;
    void consume(set<Attribute> *a, set<Table> tables, Node *source, set<string> *fixed_tables) override;

private:
    int type = map_sum;
    int virtuality = hist_att;
    Attribute op1 = Attribute("NULL", "NULL", att_int, data_att, 0);
    Attribute op2 = Attribute("NULL", "NULL", att_int, data_att, 0);
};

class JoinNode : public Node{

public:
    JoinNode(string name, Node* parent, Attribute att1, Attribute att2) : Node(name){
        this->att1 = att1;
        this->att2 = att2;
        this->parent = parent;
        this->parent->child = this;
    }
    void setLeftChild(Node* node){
        this->leftChild = node;
        this->leftChild->parent = this;
    }
    void setRightChild(Node* node){
        this->rightChild = node;
        this->rightChild->parent = this;
    }
    void produce(set<Attribute> *a, set<Table> tables, set<string> *fixed_tables) override;
    void consume(set<Attribute> *a, set<Table> tables, Node *source, set<string> *fixed_tables) override;

private:
    Attribute att1 = Attribute("NULL", "NULL", att_int, data_att, 0);
    Attribute att2 = Attribute("NULL", "NULL", att_int, data_att, 0);
    Node* leftChild;
    Node* rightChild;
    set<Attribute> la;
    set<Attribute> ra;
    set <Table> left_tables;
    set <Table> right_tables;
    bool parent_has_bracket = false;
};

///////////////////////////////////////////////////////////////////////////////////////////////////

void SelectNode::produce(set<Attribute> *a, set<Table> tables, set<string> *fixed_tables) {
    this->child->produce(a, tables, fixed_tables);
}

void SelectNode::consume(set<Attribute> *a, set<Table> tables, Node *source, set<string> *fixed_tables) {
    fprintf(pfile,"%s//APPLYING THE CONDITIONS OF EVERY SUBCONDITION INSIDE SELECT NODE\n\n", indent.c_str());
    for (int k = 0; k < int_sub_conditions; ++k) {

        int brackets = 0;
        fprintf(pfile,"%s{\n", indent.c_str());
        indent += "\t";
        brackets++;

        fprintf(pfile,"%s//TAKING A COPY FROM EVERY ATTRIBUTE SO WE COULD RETRIEVE IT AFTER THIS SUBCONDITION EXECUTION\n\n", indent.c_str());

        for (auto i = tables.begin(); i != tables.end(); ++i) {
            string new_name = this->name + "_" + i->name;
            fprintf(pfile, "%sint %s_table_size = %s_table_size;\n",
                            indent.c_str(), new_name.c_str(), i->name.c_str());
        }

        fprintf(pfile,"\n");

        for (auto i = a->begin(); i != a->end(); i++) {
            string new_name = this->name + "_" + i->name;
            if( i->virtuality == hist_att) {
                auto varname = i->name.c_str();
                auto ind = indent.c_str();
                auto tablename = i->table_name.c_str();
                fprintf(pfile, "%sHistogram %s_hist;\n"
                               "%scopy_hist(&%s_hist, &%s_hist);\n\n",
                        ind, new_name.c_str(),
                        ind, new_name.c_str(), varname);
            }
            if ( i->virtuality == data_att){
                string tp = "int";
                if(i->type == att_string)
                    tp = "string";
                fprintf(pfile, "%s%s %s = %s;\n\n", indent.c_str(), tp.c_str(), new_name.c_str(), i->name.c_str());
            }
        }

        fprintf(pfile, "\n%s//APPLYING SUBCONDITIONS\n\n", indent.c_str());




        ///int variables:

        for (int i = 0; i < int_variables[k].size(); ++i) {
            if( int_variables[k][i].virtuality == hist_att) {
                fprintf(pfile, "%s%s_table_size *= limit(&%s_hist, %i, %i);\n",
                       indent.c_str(), int_variables[k][i].table_name.c_str(), int_variables[k][i].name.c_str(),
                       int_ranges[k][i].first, int_ranges[k][i].second);
            }
            if ( int_variables[k][i].virtuality == data_att) {
                fprintf(pfile, "%sif(%i <= %s && %s < %i)\n%s{\n",indent.c_str(), int_ranges[k][i].first, int_variables[k][i].name.c_str(), int_variables[k][i].name.c_str(), int_ranges[k][i].second, indent.c_str());
                indent += "\t";
                brackets++;
            }
            fprintf(pfile, "\n");
        }



        ///string variables:

        for (int i = 0; i < string_variables[k].size(); ++i) {
            if (string_variables[k][i].virtuality == hist_att) {
//                fprintf(pfile, "%sbool %s_valids[BUCKET_COUNT] = {0};\n", indent.c_str(),
//                       string_variables[k][i].name.c_str());
                for (int l = 0; l < valid_strings[k][i].size(); ++l) {
//                    fprintf(pfile, "%s%s_valids[%i] = 1;\n", indent.c_str(), string_variables[k][i].name.c_str(),
//                           string_id[string_variables[k][i].id][valid_strings[k][i][l].c_str()]);
                    fprintf(pfile, "%s%s_table_size *= string_limit(&%s_hist, %i);\n",
                            indent.c_str(), string_variables[k][i].table_name.c_str(),
                            string_variables[k][i].name.c_str(),
                            string_id[string_variables[k][i].id][valid_strings[k][i][l].c_str()]);
                }
            }
            if ( string_variables[k][i].virtuality == data_att) {
                fprintf(pfile,"%sif(\n",indent.c_str());
                for (int l = 0; l < valid_strings[k][i].size(); ++l) {
                    fprintf(pfile, "%s   (%s == \"%s\")" , indent.c_str(), string_variables[k][i].name.c_str(), valid_strings[k][i][l].c_str());
                    if ( l != valid_strings[k][i].size() - 1)fprintf(pfile,"||");
                    fprintf(pfile,"\n");
                }
                fprintf(pfile,"%s    ){\n", indent.c_str());
                indent += "\t";
                brackets++;
            }
            fprintf(pfile,"\n");
        }

        fprintf(pfile,"\n\n");
        parent->consume(a, tables, this, fixed_tables);

        //one remaining bracket is for condition itself
        while (brackets > 1) {
            indent = indent.substr(0, indent.size() - 1);
            fprintf(pfile,"%s}\n", indent.c_str());
            brackets--;
        }


        fprintf(pfile,"%s//RETRIEVING ATTRIBUTES\n\n", indent.c_str());


        for (auto i = tables.begin(); i != tables.end(); ++i) {
            string new_name = this->name + "_" + i->name;
            fprintf(pfile, "%s%s_table_size= %s_table_size;\n",
                    indent.c_str(), i->name.c_str(), new_name.c_str());
        }

        fprintf(pfile,"\n");


        for (auto i = a->begin(); i != a->end(); i++) {
            string new_name = this->name + "_" + i->name;
            if(i->virtuality == hist_att) {
                auto varname = i->name.c_str();
                auto ind = indent.c_str();
                fprintf(pfile, "%scopy_hist(&%s_hist, &%s_hist);\n",
//                               "%s%s_ratio = %s_ratio;\n\n",
                        ind, varname, new_name.c_str()
//                        ind, varname, new_name.c_str()
                        );
            }
            if(i->virtuality == data_att) {
                fprintf(pfile, "%s%s = %s;\n\n", indent.c_str(), i->name.c_str(), new_name.c_str());
            }
        }

        fprintf(pfile,"\n");
        indent = indent.substr(0, indent.size() - 1);
        fprintf(pfile,"%s}\n", indent.c_str());
    }
}

void ScanNode::produce(set<Attribute> *a, set<Table> tables, set<string> *fixed_tables) {
    fprintf(pfile,"%s//SCANNING ATTRIBUTES(TAKING A COPY FROM DEFAULTS)\n\n", indent.c_str());
    fprintf(pfile,"%s{\n", indent.c_str());
    int brackets = 1;
    indent += "\t";
    for (auto i = a->begin(); i != a->end(); i++){
        if(i->virtuality == hist_att) {
            auto varname = i->name.c_str();
            auto ind = indent.c_str();
            auto tablename = i->table_name.c_str();
            fprintf(pfile, "%sHistogram %s_hist;\n"
                           "%s%s_hist.minimum = default_histogram[%s][%s_id].minimum;\n"
                           "%s%s_hist.maximum = default_histogram[%s][%s_id].maximum;\n"
                           "%s%s_hist.bucket_count = default_histogram[%s][%s_id].bucket_count;\n"
                           "%s%s_hist.bucket_size = default_histogram[%s][%s_id].bucket_size;\n"
                           "%sfor (int i = 0; i < %s_hist.bucket_count; ++i){\n"
                           "%s    %s_hist.bucket[i] = default_histogram[%s][%s_id].bucket[i];\n"
                           "%s}\n",
                    ind, varname,
                    ind, varname, tablename, varname,
                    ind, varname, tablename, varname,
                    ind, varname, tablename, varname,
                    ind, varname, tablename, varname,
//                    ind, varname, varname,
                    ind, varname,
                    ind, varname, tablename, varname,
                    ind);
        }
        if ( i->virtuality == data_att){
            if(fixed_tables->find(i->table_name) == fixed_tables->end()) {
                fixed_tables->insert(i->table_name);
                fprintf(pfile,"%s", indent.c_str());
                fprintf(pfile, "for (int %s_it = 0; %s_it < table_size[%s]; %s_it++)\n", i->table_name.c_str(),
                       i->table_name.c_str(), i->table_name.c_str(), i->table_name.c_str());
                fprintf(pfile,"%s{\n", indent.c_str());
                indent += "\t";
                brackets++;
            }

            if(i->type == att_int)
                fprintf(pfile, "%sint %s = int_table[%s][%s_id][%s_it];\n\n",indent.c_str(), i->name.c_str(), i->table_name.c_str(), i->name.c_str(), (i->table_name.c_str()));
            else if (i->type == att_string)
                fprintf(pfile, "%sstring %s = string_table[%s][%s_id][%s_it];\n\n",indent.c_str(), i->name.c_str(), i->table_name.c_str(), i->name.c_str(), (i->table_name.c_str()));

        }
    }

    for ( auto i = tables.begin(); i != tables.end(); i++){
        if ( fixed_tables->find(i->name) == fixed_tables->end() ){
//            cout << i->name << endl;
            fprintf(pfile,"%sdouble %s_table_size = table_size[%s];\n", indent.c_str(), i->name.c_str(), i->name.c_str());
        } else {
            fprintf(pfile,"%sdouble %s_table_size = 1;\n", indent.c_str(), i->name.c_str());
        }
    }
//    fprintf(pfile,"%sdouble %s_table_size = table_size[lineitem], part_table_size = table_size[part];\n\n", indent.c_str());



    parent->consume(a, tables, this, fixed_tables);
//    fprintf(pfile, " asdf\n");
    while (brackets-- > 0){
        indent = indent.substr(0, indent.size()-1);
        fprintf(pfile, "%s}\n", indent.c_str());
    }
}

void ScanNode::consume(set<Attribute> *a, set<Table> tables, Node *source, set<string> *fixed_tables) {}

void HashScanNode::produce(set<Attribute> *a, set<Table> tables, set<string> *fixed_tables) {

    fprintf(pfile,"%s//UNHASHING KEY ATTRIBUTE AND FINDING OTHER ATTRIBUTES IN THAT TABLE(TAKING A COPY FROM DEFAULTS)\n\n", indent.c_str());
    fprintf(pfile,"%s{\n", indent.c_str());
    int brackets = 1;
    indent += "\t";
    for (auto i = a->begin(); i != a->end(); i++){
        if(i->virtuality == hist_att) {
            auto varname = i->name.c_str();
            auto ind = indent.c_str();
            auto tablename = i->table_name.c_str();
            fprintf(pfile, "%sHistogram %s_hist;\n"
                           "%s%s_hist.minimum = default_histogram[%s][%s_id].minimum;\n"
                           "%s%s_hist.maximum = default_histogram[%s][%s_id].maximum;\n"
                           "%s%s_hist.bucket_count = default_histogram[%s][%s_id].bucket_count;\n"
                           "%s%s_hist.bucket_size = default_histogram[%s][%s_id].bucket_size;\n"
                           "%sfor (int i = 0; i < %s_hist.bucket_count; ++i){\n"
                           "%s    %s_hist.bucket[i] = default_histogram[%s][%s_id].bucket[i];\n"
                           "%s}\n",
//                           "%sfloat %s_ratio = 1.0;\n",
                    ind, varname,
                    ind, varname, tablename, varname,
                    ind, varname, tablename, varname,
                    ind, varname, tablename, varname,
                    ind, varname, tablename, varname,
                    ind, varname,
                    ind, varname, tablename, varname,
                    ind
//                    ind, varname
            );
        }
        if ( i->virtuality == data_att){
            if(fixed_tables->find(i->table_name) == fixed_tables->end()) {
                fixed_tables->insert(i->table_name);
                fprintf(pfile,"%s", indent.c_str());
                fprintf(pfile, "int %s_it = search(%s_id, %s)->data;\n", i->table_name.c_str(),
                        hashedAtt->name.c_str(), sourceAtt->name.c_str());
                fixed_tables->insert(hashedAtt->table_name);
                fprintf(pfile,"%s{\n", indent.c_str());
                indent += "\t";
                brackets++;
            }
            if(i->type == att_int)
                fprintf(pfile, "%sint %s = int_table[%s][%s_id][%s_it];\n\n",indent.c_str(), i->name.c_str(), i->table_name.c_str(), i->name.c_str(), (i->table_name.c_str()));
            else if (i->type == att_string)
                fprintf(pfile, "%sstring %s = string_table[%s][%s_id][%s_it];\n\n",indent.c_str(), i->name.c_str(), i->table_name.c_str(), i->name.c_str(), (i->table_name.c_str()));
        }
    } fprintf(pfile,"\n");

    for ( auto i = tables.begin(); i != tables.end(); i++){
        if ( fixed_tables->find(i->name) == fixed_tables->end() ){
            fprintf(pfile,"%sdouble %s_table_size = table_size[%s];\n", indent.c_str(), i->name.c_str(), i->name.c_str());
        } else {
            fprintf(pfile,"%sdouble %s_table_size = 1;\n", indent.c_str(), i->name.c_str());
        }
    }

    parent->consume(a, tables, this, fixed_tables);

    while (brackets-- > 0){
        indent = indent.substr(0, indent.size()-1);
        fprintf(pfile, "%s}\n", indent.c_str());
    }
}

void HashScanNode::consume(set<Attribute> *a, set<Table> tables, Node *source, set<string> *fixed_tables) {}

void AggregateNode::produce(set<Attribute> *a, set<Table> tables, set<string> *fixed_tables) {
    a->insert(op1);
    a->insert(op2);
    auto in = indent.c_str();
    auto nm = name.c_str();
    if( virtuality == hist_att) {
        fprintf(pfile, "%sint %s_bucket[BUCKET_COUNT];\n"
               "%sint %s_min;\n"
               "%sint %s_max;\n",
               in, nm,
               in, nm,
               in, nm);
    }
    if ( virtuality == data_att){
        fprintf(pfile, "%sdouble %s = 0;\n", in, nm);
    }
    this->child->produce(a, tables, fixed_tables);
}

void AggregateNode::consume(set<Attribute> *a, set<Table> tables, Node *source, set<string> *fixed_tables) {
    string op1_name = op1.name;
    string op2_name = op2.name;
    auto in = indent.c_str();
    auto nm = name.c_str();
    auto o1 = op1_name.c_str();
    auto o2 = op2_name.c_str();

    if (type == map_sum) {
        if (virtuality == hist_att) {
            if (op1.virtuality == hist_att && op2.virtuality == hist_att) {
                fprintf(pfile, "%s//CALCULATING MAP EXPRESSION\n\n"
                       "%s%s_hist.minimum = %s_hist.minimum + %s_hist.minimum;\n"
                       "%s%s_hist.minimum = %s_hist.minimum + %s_hist.minimum;\n"
                       "%s%s_hist.maximum = %s_hist.maximum + %s_hist.maximum;\n"
                       "%sint bs1 = (%s_hist.maximum - %s_hist.minimum)/%s_hist.bucket_count;\n"
                       "%sint bs2 = (%s_hist.maximum - %s_hist.minimum)/%s_hist.bucket_count;\n"
                       "%sint bs3 = (%s_hist.maximum - %s_hist.minimum)/%s_hist.bucket_count;\n"
                       "%sfor (int i = 0; i < %s_hist.bucket_count; ++i) {\n"
                       "%s     for (int j = 0; j < %s_hist.bucket_count; ++j) {\n"
                       "%s          int val = (%s_hist.minimum + i * bs1 + bs1/2) +\n"
                       "%s               (%s_hist.minimum + j * bs2 + bs2/2);\n"
                       "%s          %s_hist.bucket[(val - %s_hist.minimum)/bs3] += %s_hist.bucket[i] + %s_hist.bucket[j];\n"
                       "%s     }\n"
                       "%s}\n\n",
                       in,
                       in, nm, o1, o2,
                       in, nm, o1, o2,
                       in, nm, o1, o2,
                       in, o1, o1, o1,
                       in, o2, o2, o2,
                       in, nm, nm, nm,
                       in, o1,
                       in, o2,
                       in, o1,
                       in, o2,
                       in, nm, nm, o1, o2,
                       in,
                       in
                );
            }
            if ((op1.virtuality == hist_att && op2.virtuality == data_att) ||
                    (op2.virtuality == hist_att && op1.virtuality == data_att)) {
                if ( op1.virtuality == data_att){
                    string tt = o1;
                    o1 = o2;
                    o2 = tt.c_str();
                }
                fprintf(pfile, "%s//CALCULATING MAP EXPRESSION\n\n"
                       "%s%s_hist.minimum = %s_hist.minimum + %s;\n"
                       "%s%s_hist.maximum = %s_hist.maximum + %s;\n"
                       "%sfor (int i = 0; i < BUCKET_COUNT; ++i)\n"
                       "%s\t%s_hist.bucket[i] = %s_hist.bucket[i];\n",
                       in,
                       in, nm, o1, o2,
                       in, nm, o1, o2,
                       in,
                       in, nm, o1
                );
                if ( op1.virtuality == data_att){
                    string tt = o1;
                    o1 = o2;
                    o2 = tt.c_str();
                }
            }
            if( op1.virtuality == data_att && op2.virtuality == data_att){
                fprintf(pfile, "%sERROR CALCULATING MAP EXPRESSION. CAN NOT CALCULATE A HIST FROM 2 NUMBERS\n\n");
            }
        }
        if (virtuality == data_att) {
            if (op1.virtuality == hist_att && op2.virtuality == hist_att) {
                fprintf(pfile, "%s//CALCULATING MAP EXPRESSION\n\n"
                       "%sint mean1 = 0;\n"
                       "%sint mean2 = 0;\n"
                       "%sint count1 = 0;\n"
                       "%sint count2 = 0;\n"
                       "%sint bs1 = (%s_hist.maximum - %s_hist.minimum)/%s_hist.bucket_count;\n"
                       "%sint bs2 = (%s_hist.maximum - %s_hist.minimum)/%s_hist.bucket_count;\n"
                       "%sfor (int i = 0; i < %s_hist.bucket_count; ++i) {\n"
                       "%s\tcount1 += %s_hist.bucket[i];\n"
                       "%s\tcount2 += %s_hist.bucket[i];\n"
                       "%s\tmean1 += %s_hist.bucket[i] * (%s_hist.minimum + bs1 * i + bs1/2);\n"
                       "%s\tmean2 += %s_hist.bucket[i] * (%s_hist.minimum + bs2 * i + bs2/2);\n"
                       "%s\t}\n"
                       "%smean1 /= count1;\n"
                       "%smean2 /= count2;\n"
                       "%s%s += mean1 + mean2;\n"
                       "%s}\n\n",
                       in,
                       in,
                       in,
                       in,
                       in,
                       in, o1, o1, o1,
                       in, o2, o2, o2,
                       in, o1,
                       in, o1,
                       in, o2,
                       in, o1, o1,
                       in, o2, o2,
                       in,
                       in,
                       in,
                       in, nm,
                       in
                );
            }
            if ((op1.virtuality == hist_att && op2.virtuality == data_att) ||
                (op2.virtuality == hist_att && op1.virtuality == data_att)) {
                if ( op1.virtuality == data_att){
                    string tt = o1;
                    o1 = o2;
                    o2 = tt.c_str();
                }
                fprintf(pfile, "%s//CALCULATING MAP EXPRESSION\n\n"
                       "%sint mean = 0;\n"
                       "%sint count = 0;\n"
                       "%sint bs = (%s_hist.maximum - %s_hist.minimum)/BUCKET_COUNT;\n"
                       "%sfor (int i = 0; i < BUCKET_COUNT; ++i) {\n"
                       "%s\tcount += %s_hist.bucket[i];\n"
                       "%s\tmean += %s_hist.bucket[i] * (%s_hist.minimum + bs * i + bs/2);\n"
                       "%s\t}\n"
                       "%smean /= count;\n"
                       "%s%s += mean + %s;\n",
                       in,
                       in,
                       in,
                       in, o1, o1,
                       in,
                       in, o1,
                       in, o1, o1,
                       in,
                       in,
                       in, nm, o2
                );
                if ( op1.virtuality == data_att){
                    string tt = o1;
                    o1 = o2;
                    o2 = tt.c_str();
                }
            }
            if( op1.virtuality == data_att && op2.virtuality == data_att){
                fprintf(pfile, "%s//CALCULATING MAP EXPRESSION\n\n"
                       "%s%s += %s + %s;\n",
                       in,
                       in, nm, o1, o2
                );
            }
        }
    } else if ( type == map_mult ){
        if (virtuality == hist_att) {
            if (op1.virtuality == hist_att && op2.virtuality == hist_att) {
            }
            if ((op1.virtuality == hist_att && op2.virtuality == data_att) ||
                (op2.virtuality == hist_att && op1.virtuality == data_att)) {
            }
            if( op1.virtuality == data_att && op2.virtuality == data_att){
                fprintf(pfile, "%sERROR CALCULATING MAP EXPRESSION. CAN NOT CALCULATE A HISTOGRAM FROM 2 NUMBERS\n\n");
            }
        }
        if (virtuality == data_att) {
//            cout << op1.virtuality << " " << op2.virtuality << endl;
            if (op1.virtuality == hist_att && op2.virtuality == hist_att) {
//                fprintf(pfile, "%s//CALCULATING MAP EXPRESSION\n\n"
//                               "%sdouble mean1 = 0;\n"
//                               "%sdouble mean2 = 0;\n"
//                               "%sdouble count1 = 0;\n"
//                               "%sdouble count2 = 0;\n"
//                               "%sdouble bs1 = (%s_hist.maximum - %s_hist.minimum)/%s_hist.bucket_count;\n"
//                               "%sdouble bs2 = (%s_hist.maximum - %s_hist.minimum)/%s_hist.bucket_count;\n"
//                               "%sfor (int i = 0; i < %s_hist.bucket_count; ++i) {\n"
//                               "%s\tcount1 += %s_hist.bucket[i];\n"
//                               "%s\tmean1 += %s_hist.bucket[i] * (%s_hist.minimum + bs1 * i + bs1/2);\n"
//                               "%s\t}\n"
//                               "%sfor (int i = 0; i < %s_hist.bucket_count; ++i) {\n"
//                               "%s\tcount2 += %s_hist.bucket[i];\n"
//                               "%s\tmean2 += %s_hist.bucket[i] * (%s_hist.minimum + bs2 * i + bs2/2);\n"
//                               "%s\t}\n"
//                               "%smean1 /= count1;\n"
//                               "%smean2 /= count2;\n"
//                               "%s%s += mean1 * mean2;\n\n",
//                        in,
//                        in,
//                        in,
//                        in,
//                        in,
//                        in, o1, o1, o1,
//                        in, o2, o2, o2,
//                        in, o1,
//                        in, o1,
//                        in, o1, o1,
//                        in,
//                        in, o2,
//                        in, o2,
//                        in, o2, o2,
//                        in,
//                        in,
//                        in,
//                        in, nm
//                );

                fprintf(pfile,
                "%sdouble val1 = 0;\n"
                "%sdouble val2 = 0;\n"
                "%sdouble count1 = 0;\n"
                "%sdouble count2 = 0;\n"
                "%sdouble summation = 0;\n"
                "%sdouble bs1 = (%s_hist.maximum - %s_hist.minimum)/%s_hist.bucket_count;\n"
                "%sdouble bs2 = (%s_hist.maximum - %s_hist.minimum)/%s_hist.bucket_count;\n"
                "%sfor (int i = 0; i < %s_hist.bucket_count; ++i) {\n"
                "%s    count1 += %s_hist.bucket[i];\n"
                "%s}\n"
                "%sfor (int i = 0; i < %s_hist.bucket_count; ++i) {\n"
                "%s    count2 += %s_hist.bucket[i];\n"
                "%s}\n"
                "%sdouble mlt = count1 * count2;\n"

                "%sfor (int i = 0; i < %s_hist.bucket_count; ++i) {\n"
                "%s\t\tfor (int j = 0; j < %s_hist.bucket_count; ++j) {\n"
                "    %s\t\tval1 = %s_hist.bucket[i] * (%s_hist.minimum + bs1 * i + bs1/2);\n"
                "    %s\t\tval2 = %s_hist.bucket[j] * (%s_hist.minimum + bs2 * j + bs2/2);\n"
                "    %s\t\tsummation += (val1 * val2) / mlt;\n"
                "%s\t}\n"
                "%s}\n"
                "%s%s += summation",
                    in,
                    in,
                    in,
                    in,
                    in,
                    in, o1, o1, o1,
                    in, o2, o2, o2,
                    in, o1,
                    in, o1,
                    in,
                    in, o2,
                    in, o2,
                    in,
                    in,

                    in, o1,
                    in, o2,

                    in, o1, o1,
                    in, o2, o2,
                    in,
                    in,
                    in,
                    in, nm);
                for (auto at = tables.begin(); at != tables.end(); at++) {
                    fprintf(pfile, " * %s_table_size", at->name.c_str());
                }
                for (int i = 0; i < extra_ratios.size(); ++i) {
                    fprintf(pfile, " * %s_ratio", extra_ratios[i].c_str());
                }
                fprintf(pfile, ";\n");
            }
            if ((op1.virtuality == hist_att && op2.virtuality == data_att) ||
                (op2.virtuality == hist_att && op1.virtuality == data_att)) {
                if ( op1.virtuality == data_att){
                    string tt = o1;
                    o1 = o2;
                    o2 = tt.c_str();
                }
                fprintf(pfile, "%s//CALCULATING MAP EXPRESSION\n\n"
                               "%sdouble mean = 0;\n"
                               "%sdouble count = 0;\n"
                               "%sdouble bs = (%s_hist.maximum - %s_hist.minimum)/%s_hist.bucket_count;\n"
                               "%sfor (int i = 0; i < %s_hist.bucket_count; ++i) {\n"
                               "%s\tcount += %s_hist.bucket[i];\n"
                               "%s\tmean += %s_hist.bucket[i] * (%s_hist.minimum + bs * i + bs/2);\n"
                               "%s\t}\n"
                               "%smean /= count;\n"
                               "%s%s += mean * %s",
                        in,
                        in,
                        in,
                        in, o1, o1, o1,
                        in, o1,
                        in, o1,
                        in, o1, o1,
                        in,
                        in,
                        in, nm, o2
                );
                for (auto at = tables.begin(); at != tables.end(); at++) {
                    fprintf(pfile, " * %s_table_size", at->name.c_str());
                }
                for (int i = 0; i < extra_ratios.size(); ++i) {
                    fprintf(pfile, " * %s_ratio", extra_ratios[i].c_str());
                }
                fprintf(pfile, ";\n");
                if ( op1.virtuality == data_att){
                    string tt = o1;
                    o1 = o2;
                    o2 = tt.c_str();
                }
            }
            if (op1.virtuality == data_att && op2.virtuality == data_att) {
                fprintf(pfile, "%s//CALCULATING MAP EXPRESSION\n\n"
                               "%s%s += %s * %s",
                        in,
                        in, nm, o1, o2
                );

                for (auto at = tables.begin(); at != tables.end(); at++) {
                    fprintf(pfile, " * %s_table_size", at->name.c_str());
                }
                for (int i = 0; i < extra_ratios.size(); ++i) {
                    fprintf(pfile, " * %s_ratio", extra_ratios[i].c_str());
                }

//                for (auto at = tables.begin(); at != tables.end(); at++) {
//                    if(fixed_tables->find(at->name) == fixed_tables->end())
//                        fprintf(pfile, " * table_size[%s]", at->name.c_str());
//                }

                fprintf(pfile, ";\n");
            }
        }
    }
    parent->consume(a, tables, this, fixed_tables);
}

void JoinNode::produce(set<Attribute> *a, set<Table> tables, set<string> *fixed_tables) {
    a->insert(att1);
    a->insert(att2);
    la.clear();
    ra.clear();
    la.insert(att1);
    ra.insert(att2);
    for (auto i = a->begin(); i != a->end(); ++i) {
        if (i->table_name == att1.table_name)
            la.insert(*i);
        else
            ra.insert(*i);
    }
    left_tables.clear();
    right_tables.clear();
    left_tables.insert(att1.table_name);
    right_tables.insert(att2.table_name);
    this->leftChild->produce(&la, left_tables, fixed_tables);
}

void JoinNode::consume(set<Attribute> *a, set<Table> tables, Node *source, set<string> *fixed_tables) {

    if ( source == this->leftChild ){
        this->rightChild->produce(&ra, right_tables, fixed_tables);
        return;
    }

    for (auto j = la.begin(); j != la.end() ; ++j) {
        a->insert(*j);
    }

    for (auto j = left_tables.begin(); j != left_tables.end(); j++){
        tables.insert(*j);
    }

    int er_size = extra_ratios.size();

    fprintf(pfile,"%s//JOIN\n%s{\n", indent.c_str(), indent.c_str());

    indent += "\t";
    auto in = indent.c_str();
    auto nm = name.c_str();
    auto al = att1.name.c_str();
    auto ar = att2.name.c_str();
    auto altn = att1.table_name.c_str();
    auto artn = att2.table_name.c_str();

    for (auto i = tables.begin(); i != tables.end(); ++i) {
        string new_name = this->name + "_" + i->name;
        fprintf(pfile, "%sint %s_table_size = %s_table_size;\n",
                indent.c_str(), new_name.c_str(), i->name.c_str());
    }

    fprintf(pfile,"\n");

    {
        auto i = &att1;
        string new_name = this->name + "_" + i->name;
        if (i->virtuality == hist_att) {
            auto varname = i->name.c_str();
            auto ind = indent.c_str();
            auto tablename = i->table_name.c_str();
            fprintf(pfile, "%sHistogram %s_hist;\n"
                           "%scopy_hist(&%s_hist, &%s_hist);\n\n",
                    ind, new_name.c_str(),
                    ind, new_name.c_str(), varname);
        }
        i = &att2;
        new_name = this->name + "_" + i->name;
        if (i->virtuality == hist_att) {
            auto varname = i->name.c_str();
            auto ind = indent.c_str();
            auto tablename = i->table_name.c_str();
            fprintf(pfile, "%sHistogram %s_hist;\n"
                           "%scopy_hist(&%s_hist, &%s_hist);\n\n",
                    ind, new_name.c_str(),
                    ind, new_name.c_str(), varname);
        }
    }

    if ( att1.virtuality == hist_att && att2.virtuality == hist_att) {

        fprintf(pfile, "%sdouble expected_left_joins = 0;\n"
                       "%sdouble expected_right_joins = 0;\n"
                       "%sdouble bs = %s_hist.bucket_size;\n "
                       "%sfor (int i = 0; i < %s_hist.bucket_count; i++){\n"
                       "%s\tdouble bucket_left_joins = %s_hist.bucket[i]/bs;\n"
                       "%s\tdouble bucket_right_joins = %s_hist.bucket[i]/bs;\n"
                       "%s\texpected_left_joins += bucket_left_joins;\n"
                       "%s\texpected_right_joins += bucket_right_joins;\n"
                       "%s\t%s_hist.bucket[i] *= bucket_left_joins;\n"
                       "%s\t%s_hist.bucket[i] *= bucket_right_joins;\n"
                       "%s}\n"
                       "%sexpected_left_joins /= %s_hist.bucket_count;\n"
                       "%sexpected_right_joins /= %s_hist.bucket_count;\n\n"
                       "%sdouble %s_ratio = expected_left_joins / %s_table_size ;\n\n",
//                       "%s%s_table_size *= expected_right_joins;\n\n",
                in,
                in,
                in, al,
                in, al,
                in, ar,
                in, al,
                in,
                in,
                in, al,
                in, ar,
                in,
                in, al,
                in, ar,
                in, this->name.c_str(), artn);
        extra_ratios.push_back(this->name);
    }

    if ((att1.virtuality == hist_att && att2.virtuality == data_att) ||
        (att2.virtuality == hist_att && att1.virtuality == data_att)) {
        if (att1.virtuality == data_att) {
            string tt = al;
            al = ar;
            ar = tt.c_str();
            string gg = altn;
            altn = artn;
            artn = gg.c_str();
        }

        fprintf(pfile,
               "%sdouble %s_ratio = 0;\n"
               "%sif(%s_hist.minimum <= %s && %s <= %s_hist.maximum){\n"
               "%s\tint bs = %s_hist.bucket_size, total = 0;\n"
               "%s\tfor ( int i = 0 ; i < %s_hist.bucket_count; i++)\n"
               "%s\t    total += %s_hist.bucket[i];\n"
               "%s\tint buck = (double)(%s - %s_hist.minimum)/bs;\n"
               "%s\t%s_ratio = (double)%s_hist.bucket[buck]/bs/total;\n"
               "%s}\n\n",
               in, this->name.c_str(),
               in, al, ar, ar, al,
               in, al,
               in, al,
               in, al,
               in, ar, al,
               in, this->name.c_str(), al,
               in
        );
        extra_ratios.push_back(this->name);
        if (att1.virtuality == data_att) {
            string tt = al;
            al = ar;
            ar = tt.c_str();
        }
    }

    bool bracket = false;
    if( att1.virtuality == data_att && att2.virtuality == data_att){
        bracket = true;
        fprintf(pfile, "%sif (%s == %s){\n\n", in, al, ar);
        indent += "\t";
    }

    parent->consume(a, tables, this, fixed_tables);

    while (extra_ratios.size() > er_size) extra_ratios.pop_back();

    fprintf(pfile,"\n");
    if (bracket) {
        fprintf(pfile, "%s}\n", indent.c_str());
        indent = indent.substr(0, indent.size() - 1);
    }
    //this one is for join itself:
    indent = indent.substr(0, indent.size() - 1);
    for (auto i = tables.begin(); i != tables.end(); ++i) {
        string new_name = this->name + "_" + i->name;
        fprintf(pfile, "%s%s_table_size= %s_table_size;\n",
                indent.c_str(), i->name.c_str(), new_name.c_str());
    }

    fprintf(pfile,"\n");


    {
        auto i = &att1;
        string new_name = this->name + "_" + i->name;
        if(i->virtuality == hist_att) {
            auto varname = i->name.c_str();
            auto ind = indent.c_str();
            fprintf(pfile, "%scopy_hist(&%s_hist, &%s_hist);\n",
//                               "%s%s_ratio = %s_ratio;\n\n",
                    ind, varname, new_name.c_str()
//                        ind, varname, new_name.c_str()
            );
        }
        i = &att2;
        new_name = this->name + "_" + i->name;
        if(i->virtuality == hist_att) {
            auto varname = i->name.c_str();
            auto ind = indent.c_str();
            fprintf(pfile, "%scopy_hist(&%s_hist, &%s_hist);\n",
//                               "%s%s_ratio = %s_ratio;\n\n",
                    ind, varname, new_name.c_str()
//                        ind, varname, new_name.c_str()
            );
        }
    }
    fprintf(pfile,"%s}\n", indent.c_str());
}

///////////////////////////////////////////////////////////////////////////////////////////////////