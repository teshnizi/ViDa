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

enum agg_type{
    agg_set,
    agg_sum,
    agg_mult
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
class HashScanNode;
class SelectNode;
class AggregateNode;
class JoinNode;
class GroupNode;

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
    Node(string name, Node* parent) : Node(name){
        this->parent = parent;
        parent->child = this;
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

    void prep(){}

    virtual string get_class(){
        return "Node";
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
    string get_class(){
        return "ScanNode";
    }
};

class HashScanNode : public Node{
public:
    HashScanNode(string name, Node* parent, Attribute* sourceAtt,  Attribute* hashedAtt) : Node(name, parent){
        this->sourceAtt = sourceAtt;
        this->hashedAtt = hashedAtt;
    }
    void produce(set<Attribute> *a, set<Table> tables, set<string> *fixed_tables) override;
    void consume(set<Attribute> *a, set<Table> tables, Node *source, set<string> *fixed_tables) override;
    void prep(){
        fprintf(pfile, "    for (int i = 0; i < table_size[%s]; ++i) {\n"
               "        insert(%s_hash_id, int_table[%s][%s_id][i], i);\n"
               "    }\n\n", hashedAtt->table_name.c_str(), hashedAtt->name.c_str(), hashedAtt->table_name.c_str(), hashedAtt->name.c_str());
    }
    string get_class(){
        return "HashScanNode";
    }
private:
    Attribute* sourceAtt;
    Attribute* hashedAtt;
};

class SelectNode : public Node{
public:
    SelectNode(string name, Node* parent, vector<Attribute> int_variables[], int int_sub_conditions, vector<pair<int,int>> ranges[],
    int string_sub_conditions, vector<Attribute> string_variables[], vector<vector<string>> valid_strings[]) :
    Node(name, parent) {
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
    }
    void produce(set<Attribute> *a, set<Table> tables, set<string> *fixed_tables) override;
    void consume(set<Attribute> *a, set<Table> tables, Node *source, set<string> *fixed_tables) override;
    string get_class(){
        return "SelectNode";
    }

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
    AggregateNode(string name, Node* parent, int type, int virtuality, ExpressionNode* expression_root) : Node(name, parent){
        cout << "ASDF";
        this->type = type;
        this->virtuality = virtuality;
        this->expression_root = expression_root;
//        this->op1 = op1;
//        this->op2 = op2;
    }
    void produce(set<Attribute> *a, set<Table> tables, set<string> *fixed_tables) override;
    void consume(set<Attribute> *a, set<Table> tables, Node *source, set<string> *fixed_tables) override;
    int get_type(){
        return type;
    }
    string get_class(){
        return "AggregateNode";
    }
    void prep(){

        string s = name.c_str();

        if (this->child->get_class() == "GroupNode"){
            s = s + "[max_table_size]";
            auto nm = s.c_str();
            if( virtuality == hist_att) {
                fprintf(pfile, "\tint %s_bucket[BUCKET_COUNT];\n"
                               "\tint %s_min;\n"
                               "\tint %s_max;\n",
                        nm,
                        nm,
                        nm);
            }
            if ( virtuality == data_att){
                fprintf(pfile, "\tdouble %s;\n", nm);
            }
        } else {
            auto nm = s.c_str();
            if( virtuality == hist_att) {
                fprintf(pfile, "\tint %s_bucket[BUCKET_COUNT];\n"
                               "\tint %s_min;\n"
                               "\tint %s_max;\n",
                        nm,
                        nm,
                        nm);
            }
            if ( virtuality == data_att){
                fprintf(pfile, "\tdouble %s = 0;\n", nm);
            }
        }
    }
private:
    int type = agg_set;
    int virtuality = hist_att;
    ExpressionNode *expression_root;
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
    string get_class(){
        return "JoinNode";
    }
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

class GroupNode : public Node{
public:
    GroupNode(string name, Node* parent, Attribute *GB_att, vector<AggregateNode*>* aggregations) : Node(name, parent){
        this->GB_att = GB_att;
        this->aggregations = aggregations;
    }
    void produce(set<Attribute> *a, set<Table> tables, set<string> *fixed_tables) override;
    void consume(set<Attribute> *a, set<Table> tables, Node *source, set<string> *fixed_tables) override;

    string get_class(){
        return "GroupNode";
    }

    void prep(){

        fprintf(pfile,"\tint %s_it = 0;\n", this->name.c_str());

    }

private:
    Attribute *GB_att;
    vector<AggregateNode*> *aggregations;
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
                fprintf(pfile, "int %s_it = search(%s_hash_id, %s)->data;\n", i->table_name.c_str(),
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

    set<Attribute> s = expression_root->scan_atts();
    for (auto i = s.begin(); i != s.end() ; ++i) {
        a->insert(*i);
    }
//    a->insert(op2);

    this->child->produce(a, tables, fixed_tables);
}

void AggregateNode::consume(set<Attribute> *a, set<Table> tables, Node *source, set<string> *fixed_tables) {

    string op;
    if(virtuality == data_att){
        switch(type){
            case agg_set:
                op = "=";
                break;
            case agg_sum:
                op = "+=";
                break;
            case agg_mult:
                op = "*=";
                break;
            default:
                op = "=";
        }
        fprintf(pfile, "%s%s %s %s;\n", indent.c_str(), name.c_str(), op.c_str(), expression_root->run().c_str());
    }

    if(virtuality == hist_att){
        switch(type){
            case agg_set:
                op = "copy_hist";
                break;
            case agg_sum:
                if(expression_root->get_type() == "hist")
                    op = "add_hist_to_hist";
                else
                    op = "add_data_to_hist";
                break;
            case agg_mult:
                if(expression_root->get_type() == "hist")
                    op = "mult_hist_in_hist";
                else
                    op = "mult_data_in_hist";
                break;
            default:
                op = "copy_hist";
        }
        fprintf(pfile, "%s%s(*%s, *%s);\n", indent.c_str(), op.c_str(), name.c_str(), expression_root->run().c_str());

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

void GroupNode::produce(set<Attribute> *a, set<Table> tables, set<string> *fixed_tables) {
//    a->insert(*GB_att);
    this->child->produce(a, tables, fixed_tables);
}

void GroupNode::consume(set<Attribute> *a, set<Table> tables, Node *source, set<string> *fixed_tables) {

    fprintf(pfile,"%s//Grouping\n%s{\n", indent.c_str(), indent.c_str());
    indent += "\t";

    auto in = indent.c_str();

    if(GB_att->virtuality == data_att){

//        string identity_element = "0";
//        if (((AggregateNode*)(this->child))->get_type() == agg_mult)
//            identity_element = "1";

//        fprintf(pfile, "%sDataItem* %s_d = search(%s_hash_id, %s);\n"
//                       "%sif (%s_d == NULL)\n"
//                       "%s\t%s_d = insert(%s_hash_id, %s, %s);\n",
//                in, this->name.c_str(), this->name.c_str(), GB_att->name.c_str(),
//        in, this->name.c_str(),
//        in, this->name.c_str(), this->name.c_str(), GB_att->name.c_str(), identity_element.c_str());

        fprintf(pfile, "%sDataItem* %s_d = search(%s_hash_id, %s);\n"
                       "%sif (%s_d == NULL)\n"
                       "%s\t%s_d = insert(%s_hash_id, %s, %s_it++);\n"
                       "%sint %s_id = %s_d->data;\n",
                in, this->name.c_str(), this->name.c_str(), GB_att->name.c_str(),
                in, this->name.c_str(),
                in, this->name.c_str(), this->name.c_str(), GB_att->name.c_str(), this->name.c_str(),
                in, this->name.c_str(), this->name.c_str());

        for (int i = 0; i < aggregations->size(); ++i) {
            string tmp = (*aggregations)[i]->name;
            (*aggregations)[i]->name += "[" + this->name + "_id]";
            (*aggregations)[i]->consume(a, tables, this, fixed_tables);
            (*aggregations)[i]->name = tmp;
        }

    }


//    parent->consume(a, tables, this, fixed_tables);

    indent = indent.substr(0, indent.size()-1);
    fprintf(pfile, "%s}\n", indent.c_str());
}

///////////////////////////////////////////////////////////////////////////////////////////////////