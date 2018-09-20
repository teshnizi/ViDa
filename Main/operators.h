#include <iostream>
#include <bitset>
#include <vector>
#include <stdio.h>
#include "tuples2.h"
#include <set>
#include "statistics.h"
#include <map>

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

enum attribute_type{
    att_int,
    att_string
};

enum attribute_virtuality{
    data_att,
    hist_att
};

const int condition_max_subconditions = 30;

map <string, int> string_id[ENUM_COUNT];

struct Attribute{
    Attribute(string table_name, string name, int virtuality, int type, int id){
        this->name = name;
        this->table_name = table_name;
        this->type = type;
        this->virtuality = virtuality;
        this->id = id;
    }
    string name;
    string table_name;
    int type;
    int virtuality;
    int id;

    bool operator<(const Attribute& a) const{
        if(this->table_name == a.table_name){
            return this->name < a.name;
        }
        return this->table_name < a.table_name;
    }
    bool operator==(const Attribute& a) const{
        return (this->table_name == a.table_name) && (this->table_name == a.table_name);
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

    virtual void produce(set<Attribute> *a){
        this->child->produce(a);
        while(!indent.empty()) {
            indent = indent.substr(0, indent.size()-1);
//            cout << indent << "}\n";
            fprintf(pfile,"%s}\n", indent.c_str());
        }
    };

    virtual void consume(set<Attribute> *a, Node *source) {
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
    void produce(set<Attribute> *a) override;
    void consume(set<Attribute> *a, Node *source) override;
};

class HashScanNode : public Node{
public:
    HashScanNode(string name, Node* parent, Attribute* sourceAtt,  Attribute* hashedAtt) : Node(name){
        this->parent = parent;
        parent->child = this;
        this->sourceAtt = sourceAtt;
        this->hashedAtt = hashedAtt;
    }
    void produce(set<Attribute> *a) override;
    void consume(set<Attribute> *a, Node *source) override;

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
    void produce(set<Attribute> *a) override;
    void consume(set<Attribute> *a, Node *source) override;

private:
    int int_sub_conditions;
    int string_sub_conditions;
    int brackets = 0;
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
    void produce(set<Attribute> *a) override;
    void consume(set<Attribute> *a, Node *source) override;

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
    void produce(set<Attribute> *a) override;
    void consume(set<Attribute> *a, Node *source) override;

private:
    Attribute att1 = Attribute("NULL", "NULL", att_int, data_att, 0);
    Attribute att2 = Attribute("NULL", "NULL", att_int, data_att, 0);
    Node* leftChild;
    Node* rightChild;
    set<Attribute> la;
    set<Attribute> ra;
};

///////////////////////////////////////////////////////////////////////////////////////////////////

void SelectNode::produce(set<Attribute> *a) {
    this->child->produce(a);
}

void SelectNode::consume(set<Attribute> *a, Node *source) {
//    fprintf(pfile, "%svector<int> %s_att[%i];\n", indent.c_str(), this->name.c_str(), int_sub_conditions);
//    fprintf(pfile, "%srange<pair<int,int>> %s_range[%i];\n", indent.c_str(), this->name.c_str(), int_sub_conditions);
//    for (int i = 0; i < int_sub_conditions; ++i) {
//        for (int j = 0; j < int_variables[i].size(); ++j) {
//            fprintf(pfile, "%s%s_att[%i].push_back(%s);\n", indent.c_str(), this->name.c_str(), i, int_variables[i][j].name.c_str());
//            fprintf(pfile, "%s%s_range[%i].push_back(make_pair(%i,%i));\n", indent.c_str(), this->name.c_str(), i, int_ranges[i][j].first, int_ranges[i][j].second);
//        }
//    }cout << endl;

//    cout << indent << "//APPLYING THE CONDITIONS OF EVERY SUBCONDITION INSIDE SELECT NODE\n\n";
    fprintf(pfile,"%s//APPLYING THE CONDITIONS OF EVERY SUBCONDITION INSIDE SELECT NODE\n\n", indent.c_str());
    for (int k = 0; k < int_sub_conditions; ++k) {
//        cout << indent << "{\n";
        fprintf(pfile,"%s{\n", indent.c_str());
        indent += "\t";
        brackets++;

//        cout << indent << "//TAKING A COPY FROM EVERY ATTRIBUTE SO WE COULD RETRIEVE IT AFTER THIS SUBCONDITION EXECUTION\n\n";
        fprintf(pfile,"%s//TAKING A COPY FROM EVERY ATTRIBUTE SO WE COULD RETRIEVE IT AFTER THIS SUBCONDITION EXECUTION\n\n", indent.c_str());
        for (auto i = a->begin(); i != a->end(); i++) {
            string new_name = this->name + "_" + i->name;

            if( i->virtuality == hist_att) {

                auto varname = i->name.c_str();
                auto ind = indent.c_str();
                auto tablename = i->table_name.c_str();
                fprintf(pfile, "%sHistogram %s_hist;\n"
                               "%scopy_hist(&%s_hist, &%s_hist);\n",
                        ind, new_name.c_str(),
                        ind, new_name.c_str(), varname);

//                fprintf(pfile, "%sint %s_bucket[BUCKET_COUNT];\n", indent.c_str(), new_name.c_str());
//                fprintf(pfile, "%sfor (int i = 0 ; i < BUCKET_COUNT; i++)\n", indent.c_str());
//                fprintf(pfile, "%s    %s_bucket[i] = %s_bucket[i];\n", indent.c_str(), new_name.c_str(), i->name.c_str());
//                fprintf(pfile, "%sint %s_min = %s_min;\n", indent.c_str(), new_name.c_str(), i->name.c_str());
//                fprintf(pfile, "%sint %s_max = %s_max;\n", indent.c_str(), new_name.c_str(), i->name.c_str());
//                fprintf(pfile, "%sint %s_table_size = table_size[%s];\n\n", indent.c_str(),
//                       (this->name + "_" + i->table_name).c_str(), i->table_name.c_str());

            }
            if ( i->virtuality == data_att){
                string tp = "int";
                if(i->type == att_string)
                    tp = "string";
                fprintf(pfile, "%s%s %s = %s;\n\n", indent.c_str(), tp.c_str(), new_name.c_str(), i->name.c_str());
            }
        }
//        cout << endl;
//        cout << indent << "//APPLYING SUBCONDITIONS\n\n";
        fprintf(pfile, "\n%s//APPLYING SUBCONDITIONS\n\n", indent.c_str());

        for (int i = 0; i < int_variables[k].size(); ++i) {
            if( int_variables[k][i].virtuality == hist_att) {
                fprintf(pfile, "%sdouble %s_ratio = limit(%s_hist, %i, %i);\n",
                       indent.c_str(), int_variables[k][i].name.c_str(), int_variables[k][i].name.c_str(),
                       int_ranges[k][i].first, int_ranges[k][i].second);
                for (auto j = a->begin(); j != a->end(); ++j) {
                    if (j->virtuality == hist_att && int_variables[k][i].name != j->name && j->table_name == int_variables[k][i].table_name) {
                        fprintf(pfile, "%sfor (int i = 0; i < BUCKET_COUNT; i++)\n"
                               "%s       %s_hist.bucket[i] *= %s_ratio;\n",
                               indent.c_str(),
                               indent.c_str(), j->name.c_str(), int_variables[k][i].name.c_str());
                    }
                }
            }
            if ( int_variables[k][i].virtuality == data_att) {
//                cout << indent;
                fprintf(pfile, "%sif(%i < %s && %s < %i)\n%s{\n",indent.c_str(), int_ranges[k][i].first, int_variables[k][i].name.c_str(), int_variables[k][i].name.c_str(), int_ranges[k][i].second, indent.c_str());
//                cout << indent <<"{\n";
                indent += "\t";
                brackets++;
            }
//            cout << endl;
            fprintf(pfile, "\n");
        }
        for (int i = 0; i < string_variables[k].size(); ++i) {
            if (string_variables[k][i].virtuality == hist_att) {
                fprintf(pfile, "%sbool %s_valids[BUCKET_COUNT] = {0};\n", indent.c_str(),
                       string_variables[k][i].name.c_str());
                for (int l = 0; l < valid_strings[k][i].size(); ++l) {
                    fprintf(pfile, "%s%s_valids[%i] = 1;\n", indent.c_str(), string_variables[k][i].name.c_str(),
                           string_id[string_variables[k][i].id][valid_strings[k][i][l].c_str()]);
                }
                fprintf(pfile, "%sdouble %s_ratio = string_limit(%s_hist, %s_valids);\n",
                       indent.c_str(), string_variables[k][i].name.c_str(), string_variables[k][i].name.c_str(),
                       string_variables[k][i].name.c_str());
                for (auto j = a->begin(); j != a->end(); ++j) {
                    if (j->virtuality == hist_att && string_variables[k][i].name != j->name && j->table_name == string_variables[k][i].table_name) {
                        fprintf(pfile, "%sfor (int i = 0; i < %s_hist.bucket_count; i++)\n"
                               "%s       %s_hist.bucket[i] *= %s_ratio;\n",
                               indent.c_str(), j->name.c_str(),
                               indent.c_str(), j->name.c_str(), string_variables[k][i].name.c_str());
                    }
                }
            }
            if ( string_variables[k][i].virtuality == data_att) {
                fprintf(pfile,"%sif(\n",indent.c_str());
//                cout << indent << "if(\n";
                for (int l = 0; l < valid_strings[k][i].size(); ++l) {
                    fprintf(pfile, "%s   (%s == \"%s\")" , indent.c_str(), string_variables[k][i].name.c_str(), valid_strings[k][i][l].c_str());
                    if ( l != valid_strings[k][i].size() - 1)fprintf(pfile,"||");
                    fprintf(pfile,"\n");
                }
                fprintf(pfile,"%s    ){\n", indent.c_str());
//                cout << indent <<"   ){\n";
                indent += "\t";
                brackets++;
            }
//            cout << endl;
            fprintf(pfile,"\n");
        }
//        cout << endl << endl;
        fprintf(pfile,"\n\n");
        parent->consume(a, this);

        //one remaining bracket is for condition itself
        while (brackets > 1) {
            indent = indent.substr(0, indent.size() - 1);
//            cout << indent << "}\n";
            fprintf(pfile,"%s}\n", indent.c_str());
            brackets--;
        }

//        cout << indent << "//RETRIEVING ATTRIBUTES\n\n";
        fprintf(pfile,"%s//RETRIEVING ATTRIBUTES\n\n", indent.c_str());

        for (auto i = a->begin(); i != a->end(); i++) {
            string new_name = this->name + "_" + i->name;
            if(i->virtuality == hist_att) {
                auto varname = i->name.c_str();
                auto ind = indent.c_str();
                fprintf(pfile, "%scopy_hist(&%s_hist, &%s_hist);\n",
                        ind, varname, new_name.c_str());

//                fprintf(pfile, "%sfor (int i = 0 ; i < BUCKET_COUNT; i++)\n", indent.c_str());
//                fprintf(pfile, "%s    %s_bucket[i] = %s_bucket[i];\n", indent.c_str(), i->name.c_str(), new_name.c_str());
//                fprintf(pfile, "%s%s_min = %s_min;\n", indent.c_str(), i->name.c_str(), new_name.c_str());
//                fprintf(pfile, "%s%s_max = %s_max;\n\n", indent.c_str(), i->name.c_str(), new_name.c_str());
            }
            if(i->virtuality == data_att) {
                fprintf(pfile, "%s%s = %s;\n\n", indent.c_str(), i->name.c_str(), new_name.c_str());
            }
        }
//        cout << endl;
        fprintf(pfile,"\n");
        indent = indent.substr(0, indent.size() - 1);
        fprintf(pfile,"%s}\n", indent.c_str());
//        cout << indent << "}\n";
        brackets--;
    }
}

void ScanNode::produce(set<Attribute> *a) {
    fprintf(pfile,"%s//SCANNING ATTRIBUTES(TAKING A COPY FROM DEFAULTS)\n\n", indent.c_str());
    fprintf(pfile,"%s{\n", indent.c_str());

    indent += "\t";
    set<string> tables;
    for (auto i = a->begin(); i != a->end(); i++){
        if(i->virtuality == hist_att) {
            auto varname = i->name.c_str();
            auto ind = indent.c_str();
            auto tablename = i->table_name.c_str();
            fprintf(pfile, "%sHistogram %s_hist;\n"
                           "%s           %s_hist.minimum = default_histogram[%s][%s_id].minimum;\n"
                           "%s           %s_hist.maximum = default_histogram[%s][%s_id].maximum;\n"
                           "%s           %s_hist.bucket_count = default_histogram[%s][%s_id].bucket_count;\n"
                           "%s           %s_hist.bucket_size = default_histogram[%s][%s_id].bucket_size;\n"
                           "%s           %s_hist.bucket = (float*)malloc(%s_hist.bucket_count * sizeof(int));\n"
                           "%s           for (int i = 0; i < %s_hist.bucket_count; ++i){\n"
                           "%s               %s_hist.bucket[i] = default_histogram[%s][%s_id].bucket[i];\n"
                           "%s           }\n\n",
                    ind, varname,
                    ind, varname, tablename, varname,
                    ind, varname, tablename, varname,
                    ind, varname, tablename, varname,
                    ind, varname, tablename, varname,
                    ind, varname, varname,
                    ind, varname,
                    ind, varname, tablename, varname,
                    ind);
//            fprintf(pfile, "%sint %s_bucket[BUCKET_COUNT];\n", indent.c_str(), i->name.c_str());
//            fprintf(pfile, "%sfor (int i = 0 ; i < BUCKET_COUNT; i++)\n", indent.c_str());
//            fprintf(pfile, "%s    %s_bucket[i] = default_histogram[%s][%s_id].bucket[i];\n", indent.c_str(), i->name.c_str(),
//                   i->table_name.c_str(), i->name.c_str());
//            fprintf(pfile, "%sint %s_min = default_histogram[%s][%s_id].minimum;\n", indent.c_str(), i->name.c_str(), i->table_name.c_str(),
//                   i->name.c_str());
//            fprintf(pfile, "%sint %s_max = default_histogram[%s][%s_id].maximum;\n", indent.c_str(), i->name.c_str(), i->table_name.c_str(),
//                   i->name.c_str());
//            fprintf(pfile,"\n");
        }
        if ( i->virtuality == data_att){
            if(tables.find(i->table_name) == tables.end()) {
                tables.insert(i->table_name);
                fprintf(pfile,"%s", indent.c_str());
                fprintf(pfile, "for (int %s_it = 0; %s_it < table_size[%s]; %s_it++)\n", i->table_name.c_str(),
                       i->table_name.c_str(), i->table_name.c_str(), i->table_name.c_str());
                fprintf(pfile,"%s{\n", indent.c_str());
                indent += "\t";
            }
            if(i->type == att_int)
                fprintf(pfile, "%sint %s = int_table[%s][%s_id][%s_it];\n\n",indent.c_str(), i->name.c_str(), i->table_name.c_str(), i->name.c_str(), (i->table_name.c_str()));
            else if (i->type == att_string)
                fprintf(pfile, "%sstring %s = string_table[%s][%s_id][%s_it];\n\n",indent.c_str(), i->name.c_str(), i->table_name.c_str(), i->name.c_str(), (i->table_name.c_str()));

        }
    } fprintf(pfile,"\n");
    parent->consume(a, this);
}

void ScanNode::consume(set<Attribute> *a, Node *source) {}

void HashScanNode::produce(set<Attribute> *a) {

    fprintf(pfile,"%s//UNHASHING KEY ATTRIBUTE AND FINDING OTHER ATTRIBUTES IN THAT TABLE(TAKING A COPY FROM DEFAULTS)\n\n", indent.c_str());
    fprintf(pfile,"%s{\n", indent.c_str());

    indent += "\t";
    set<string> tables;
    for (auto i = a->begin(); i != a->end(); i++){
        if(i->virtuality == hist_att) {
            auto varname = i->name.c_str();
            auto ind = indent.c_str();
            auto tablename = i->table_name.c_str();
            fprintf(pfile, "%sHistogram %s_hist;\n"
                           "%s           %s_hist.minimum = default_histogram[%s][%s_id].minimum;\n"
                           "%s           %s_hist.maximum = default_histogram[%s][%s_id].maximum;\n"
                           "%s           %s_hist.bucket_count = default_histogram[%s][%s_id].bucket_count;\n"
                           "%s           %s_hist.bucket_size = default_histogram[%s][%s_id].bucket_size;\n"
                           "%s           %s_hist.bucket = (float*)malloc(%s_hist.bucket_count * sizeof(int));\n"
                           "%s           for (int i = 0; i < %s_hist.bucket_count; ++i){\n"
                           "%s               %s_hist.bucket[i] = default_histogram[%s][%s_id].bucket[i];\n"
                           "%s           }\n\n",
                    ind, varname,
                    ind, varname, tablename, varname,
                    ind, varname, tablename, varname,
                    ind, varname, tablename, varname,
                    ind, varname, tablename, varname,
                    ind, varname, varname,
                    ind, varname,
                    ind, varname, tablename, varname,
                    ind);

//            fprintf(pfile, "%sint %s_bucket[BUCKET_COUNT];\n", indent.c_str(), i->name.c_str());
//            fprintf(pfile, "%sfor (int i = 0 ; i < default_buckets; i++)\n", indent.c_str());
//            fprintf(pfile, "%s    %s_bucket[i] = default_bucket[%s][%s][i];\n", indent.c_str(), i->name.c_str(),
//                    i->table_name.c_str(), i->name.c_str());
//            fprintf(pfile, "%sint %s_min = default_minimum[%s][%s];\n", indent.c_str(), i->name.c_str(), i->table_name.c_str(),
//                    i->name.c_str());
//            fprintf(pfile, "%sint %s_max = default_maximum[%s][%s];\n", indent.c_str(), i->name.c_str(), i->table_name.c_str(),
//                    i->name.c_str());
//            fprintf(pfile,"\n");
        }
        if ( i->virtuality == data_att){
            if(tables.find(i->table_name) == tables.end()) {
                tables.insert(i->table_name);
                fprintf(pfile,"%s", indent.c_str());
                fprintf(pfile, "int %s_it = search(%s_id, %s)->data;\n", i->table_name.c_str(),
                        hashedAtt->name.c_str(), sourceAtt->name.c_str());
                fprintf(pfile,"%s{\n", indent.c_str());
                indent += "\t";
            }
            if(i->type == att_int)
                fprintf(pfile, "%sint %s = int_table[%s][%s_id][%s_it];\n\n",indent.c_str(), i->name.c_str(), i->table_name.c_str(), i->name.c_str(), (i->table_name.c_str()));
            else if (i->type == att_string)
                fprintf(pfile, "%sstring %s = string_table[%s][%s_id][%s_it];\n\n",indent.c_str(), i->name.c_str(), i->table_name.c_str(), i->name.c_str(), (i->table_name.c_str()));
        }
    } fprintf(pfile,"\n");
    parent->consume(a, this);
}

void HashScanNode::consume(set<Attribute> *a, Node *source){

}

void AggregateNode::produce(set<Attribute> *a) {
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
        fprintf(pfile, "%slong %s = 0;\n", in, nm);
    }
    this->child->produce(a);
}

void AggregateNode::consume(set<Attribute> *a, Node *source) {
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
                       "%s%s_min = %s_min + %s_min;\n"
                       "%s%s_max = %s_max + %s_max;\n"
                       "%sint bs1 = (%s_max - %s_min)/BUCKET_COUNT;\n"
                       "%sint bs2 = (%s_max - %s_min)/BUCKET_COUNT;\n"
                       "%sint bs3 = (%s_max - %s_min)/BUCKET_COUNT;\n"
                       "%sfor (int i = 0; i < BUCKET_COUNT; ++i) {\n"
                       "%s     for (int j = 0; j < BUCKET_COUNT; ++j) {\n"
                       "%s          int val = (%s_min + i * bs1 + bs1/2) +\n"
                       "%s               (%s_min + j * bs2 + bs2/2);\n"
                       "%s          %s_hist.bucket[(val - %s_min)/bs3] += %s_hist.bucket[i] + %s_hist.bucket[j];\n"
                       "%s     }\n"
                       "%s}\n\n",
                       in,
                       in, nm, o1, o2,
                       in, nm, o1, o2,
                       in, o1, o1,
                       in, o2, o2,
                       in, nm, nm,
                       in,
                       in,
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
                       "%s%s_min = %s_min + %s;\n"
                       "%s%s_max = %s_max + %s;\n"
                       "%sfor (int i = 0; i < BUCKET_COUNT; ++i)\n"
                       "%s\t%s_bucket[i] = %s_bucket[i];\n",
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
                       "%sint bs1 = (%s_max - %s_min)/BUCKET_COUNT;\n"
                       "%sint bs2 = (%s_max - %s_min)/BUCKET_COUNT;\n"
                       "%sfor (int i = 0; i < BUCKET_COUNT; ++i) {\n"
                       "%s\tcount1 += %s_bucket[i];\n"
                       "%s\tcount2 += %s_bucket[i];\n"
                       "%s\tmean1 += %s_bucket[i] * (%s_min + bs1 * i + bs1/2);\n"
                       "%s\tmean2 += %s_bucket[i] * (%s_min + bs2 * i + bs2/2);\n"
                       "%s\t}\n"
                       "%smean1 /= count1;\n"
                       "%smean2 /= count2;\n"
                       "%s%s = mean1 + mean2;\n"
                       "%s}\n\n",
                       in,
                       in,
                       in,
                       in,
                       in,
                       in, o1, o1,
                       in, o2, o2,
                       in,
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
                       "%sint bs = (%s_max - %s_min)/BUCKET_COUNT;\n"
                       "%sfor (int i = 0; i < BUCKET_COUNT; ++i) {\n"
                       "%s\tcount += %s_bucket[i];\n"
                       "%s\tmean += %s_bucket[i] * (%s_min + bs * i + bs/2);\n"
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
                fprintf(pfile, "%sERROR CALCULATING MAP EXPRESSION. CAN NOT CALCULATE A HIST FROM 2 NUMBERS\n\n");
            }
        }
        if (virtuality == data_att) {
            if (op1.virtuality == hist_att && op2.virtuality == hist_att) {
            }
            if ((op1.virtuality == hist_att && op2.virtuality == data_att) ||
                (op2.virtuality == hist_att && op1.virtuality == data_att)) {
            }
            if (op1.virtuality == data_att && op2.virtuality == data_att) {
                fprintf(pfile, "%s//CALCULATING MAP EXPRESSION\n\n"
                               "%s%s += %s * %s;\n",
                        in,
                        in, nm, o1, o2
                );
            }
        }
    }
    parent->consume(a, this);
}

void JoinNode::produce(set<Attribute> *a){
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
    this->leftChild->produce(&la);
}

void JoinNode::consume(set<Attribute> *a, Node *source) {

//    cout << indent << "//JOIN\n";
//    cout << indent << "{\n";
    if (source == this->rightChild)
        return;
    if ( source == this->leftChild ){
        this->rightChild->produce(&ra);
    }

    fprintf(pfile,"%s//JOIN\n%s{\n", indent.c_str(), indent.c_str());

    indent += "\t";
    auto in = indent.c_str();
    auto nm = name.c_str();
    auto al = att1.name.c_str();
    auto ar = att2.name.c_str();

    if ( att1.virtuality == hist_att && att2.virtuality == hist_att) {
        fprintf(pfile, "%sdouble expected_left_joins = 0;\n"
               "%sdouble expected_right_joins = 0;\n"
               "%sdouble bs = (%s_hist.max - %s_hist.min)/%s_hist.bucket_count; //or (%s_hist.max - %s_hist.min)/BUCKET_SIZE\n "
               "%sfor (int i = 0; i < %s_hist.bucket_count; i++){\n"
               "%s\tdouble bucket_left_joins = %s_hist.bucket[i]/bs;\n"
               "%s\tdouble bucket_right_joins = %s_hist.bucket[i]/bs;\n"
               "%s\texpected_left_joins += bucket_left_joins;\n"
               "%s\texpected_right_joins += bucket_right_joins;\n"
               "%s\t%s_hist.bucket[i] *= bucket_left_joins;\n"
               "%s\t%s_hist.bucket[i] *= bucket_right_joins;\n"
               "%s}\n"
               "%sexpected_left_joins /= %s_hist.bucket_count;\n"
               "%sexpected_right_joins /= %s_hist.bucket_count;\n\n",
               in,
               in,
               in, al, al, al, ar, ar,
               in, al,
               in, ar,
               in, al,
               in,
               in,
               in, al,
               in, ar,
               in,
               in, al,
               in, ar);
        for (auto i = a->begin(); i != a->end(); ++i) {
            if (i->table_name == att1.table_name) {
                fprintf(pfile, "%sfor (int i = 0; i < BUCKET_COUNT; i++)\n"
                       "%s\t%s_bucket[i] *= expected_left_joins;\n\n",
                       in,
                       in, i->name.c_str());
            }
            if (i->table_name == att2.table_name) {
                fprintf(pfile, "%sfor (int i = 0; i < BUCKET_COUNT; i++)\n"
                       "%s\t%s_bucket[i] *= expected_right_joins;\n\n",
                       in,
                       in, i->name.c_str());
            }
        }
    }

    if ((att1.virtuality == hist_att && att2.virtuality == data_att) ||
        (att2.virtuality == hist_att && att1.virtuality == data_att)) {
        if (att1.virtuality == data_att) {
            string tt = al;
            al = ar;
            ar = tt.c_str();
        }
        fprintf(pfile, "%sdouble ratio = 1;\n"
               "%sif(%s < %s_min || %s_max < %s)ratio = 0;\n"
               "%selse{\n"
               "%s\tint count = 0;\n"
               "%s\tint bs = (%s_max - %s_min)/BUCKET_COUNT;\n"
               "%s\tfor (int i = 0; i < BUCKET_COUNT; ++i) {\n"
               "%s\t\tcount += %s_bucket[i];\n"
               "%s\t}\n"
               "%s\tdouble buck = (double)(%s - %s_min)/bs;\n"
               "%s\tratio = (double)%s_bucket[buck] / (double)count;\n\n"
               "%s}\n",
               in,
               in, ar, al, al, ar,
               in,
               in,
               in, al, al,
               in,
               in, al,
               in,
               in, ar, al,
               in, al,
               in
        );
        for (auto i = a->begin(); i != a->end(); ++i) {
            if (i->table_name == att1.table_name && i->virtuality == hist_att) {
                fprintf(pfile, "%sfor (int i = 0; i < BUCKET_COUNT; i++)\n"
                       "%s\t%s_bucket[i] *= ratio;\n\n",
                       in,
                       in, i->name.c_str());
            }
        }
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
    parent->consume(a, this);
//    cout << endl;
    fprintf(pfile,"\n");
    if (bracket) {
        fprintf(pfile, "%s}\n", indent.c_str());
        indent = indent.substr(0, indent.size() - 1);
    }
    //this one is for join itself:
    indent = indent.substr(0, indent.size() - 1);
//    cout << indent << "}\n";
    fprintf(pfile,"%s}\n", indent.c_str());
}

///////////////////////////////////////////////////////////////////////////////////////////////////