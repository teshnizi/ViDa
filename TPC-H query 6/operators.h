#include <iostream>
#include <bitset>
#include <vector>
#include "tuples2.h"
#include <set>
#include "statistics.h"

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

const int condition_max_subconditions = 30;

struct Attribute{
    Attribute(string table_name, string name, int type){
        this->name = name;
        this->table_name = table_name;
        this->type = type;
    }
    string name;
    string table_name;
    int type;

    bool operator<(const Attribute& a) const{
        if(this->table_name == a.table_name){
            return this->name < a.name;
        }
        return this->table_name < a.table_name;
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
int brackets = 0;

class Node;
class DataScanNode;
class DataSelectNode;
class MapNode;
class HistScanNode;
class HistSelectNode;

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
            cout << indent << "}\n";
        }
    };

    virtual void consume(set<Attribute> *a){
        return;
    }
    void set_child(Node* child){
        this->child = child;
    }
    Node* child{};
    Node* parent{};
    string name;
};
//
//class DataScanNode : public Node{
//public:
//    DataScanNode(Node* parent, string name, int sz){
//        this->sz = sz;
//        this->name = name;
//        this->parent = parent;
//    }
//    void produce(set<string> *a) override ;
//
//private:
//    int sz;
//    string name;
//};
//
//class DataSelectNode : public Node{
//public:
//    DataSelectNode(Node* parent, vector<string> variables, vector<pair<int,int>> ranges){
//        this->variables = variables;
//        this->ranges = ranges;
//        this->parent = parent;
//    }
//    void produce(set<string> *a) override;
//
//private:
//    vector<string> variables;
//    vector<pair<int, int>> ranges;
//};
//
//class MapNode : public Node{
//public:
//    MapNode(Node* parent, string expression, string ans_name){
//        if(expression.substr(0,3) == "sum"){
//            agg_type = agg_sum;
//            expression = expression.substr(3,expression.size());
//        }
//        else if(expression.substr(0,4) == "mean"){
//            agg_type = agg_mean;
//            expression = expression.substr(4,expression.size());
//        } else {
//            agg_type = nothing;
//        }
//
//        this->expression = expression;
//        this->parent = parent;
//        this->ans_name = ans_name;
//    }
//    void produce(set<string> *a) override;
//
//private:
//    string expression;
//    string ans_name;
//    bool agg_type;
//};
//
//class DataAggregateNode : public Node{
//public:
//    DataAggregateNode(string name, vector<string>* aggregate_atts, Node* parent){
//        this->parent = parent;
//        this->name = name;
//        this->aggregate_atts = aggregate_atts;
//    }
//    void produce(set<string> *a) override;
//
//private:
//    string name;
//    vector<string> *aggregate_atts;
//};

class HistScanNode :public Node{
public:
    HistScanNode(string name, Node* parent) : Node(name){
        this->parent = parent;
        parent->child = this;
    }
    void produce(set<Attribute> *a) override;
    void consume(set<Attribute> *a) override;
};

class HistSelectNode : public Node{
public:
    HistSelectNode(string name, Node* parent, vector<Attribute> variables[], int sub_condition_count, vector<pair<int,int>> ranges[]) :
    Node(name) {
        this->sub_conditions_count = sub_condition_count;
        for (int i = 0; i < sub_condition_count; ++i) {
            this->variables[i] = variables[i];
            this->ranges[i] = ranges[i];
        }
        this->parent = parent;
        this->parent->child = this;
    }
    void produce(set<Attribute> *a) override;
    void consume(set<Attribute> *a) override;

private:
    int sub_conditions_count;
    vector<Attribute> variables[condition_max_subconditions];
    vector<pair<int, int>> ranges[condition_max_subconditions];
};

class HistMapNode : public Node{
public:
    HistMapNode(string name, Node* parent, int type, Attribute op1, Attribute op2) : Node(name){
        this->parent = parent;
        this->parent->child = this;
        this->type = type;
        this->op1 = op1;
        this->op2 = op2;
    }
    void produce(set<Attribute> *a) override;
    void consume(set<Attribute> *a) override;

private:
    int type;
    Attribute op1 = Attribute("NULL", "NULL", att_int);
    Attribute op2 = Attribute("NULL", "NULL", att_int);
};

class HistHistJoinNode : public Node{
public:
    HistHistJoinNode(string name, Node* parent, Attribute att1, Attribute att2) : Node(name){
        this->att1 = att1;
        this->att2 = att2;
        this->parent = parent;
        this->parent->child = this;
    }
    void produce(set<Attribute> *a) override;
    void consume(set<Attribute> *a) override;

private:
    Attribute att1 = Attribute("NULL", "NULL", att_int);
    Attribute att2 = Attribute("NULL", "NULL", att_int);
};

///////////////////////////////////////////////////////////////////////////////////////////////////

//void DataSelectNode::produce(set<string> *a) {
//    for (int i = 0; i < variables.size(); ++i) {
//        a->insert(variables[i]);
//    }
//    this->child->produce(a);
//    for (int i = 0; i < variables.size(); ++i) {
//        cout << indent;
//        printf("if(%i < %s && %s < %i)\n", ranges[i].first, variables[i].c_str(), variables[i].c_str(), ranges[i].second);
//    }
//    cout << indent <<"{\n";
//    indent += "\t";
//    brackets++;
//}
//
//void DataScanNode::produce(set<string> *a) {
//    cout << indent;
//    printf("for (int %s_it = 0; %s_it < %d; %s_it++)\n", name.c_str(), name.c_str(), sz, name.c_str());
//    cout << "{\n";
//    indent += "\t";
//    for (auto i = a->begin(); i != a->end(); ++i) {
//        cout << indent;
//        printf("%s = table[l_%s][%s_it];\n", (*i).c_str(), (*i).c_str(), (this->name).c_str());
//    }cout << endl;
//    brackets++;
//}
//
//void MapNode::produce(set<string> *a) {
//    vector<string> v = expression_parser(this->expression);
//    for (int i = 0; i < v.size(); ++i) {
//        a->insert(v[i]);
//    }
//    cout << indent;
//    printf("int %s = 0;\n", this->ans_name.c_str());
//    this->child->produce(a);
//    char ch=' ';
//    if ( this->agg_type != nothing)ch = '+';
//    printf("%s%s %c= %s;\n", indent.c_str(), (this->ans_name).c_str(), ch, (this->expression).c_str());
//    a->insert(this->ans_name);
//}
//
//void DataAggregateNode::produce(set<string> *a) {
//    cout<<indent << "materialize(" << name << ")\n";
//}
//

void HistSelectNode::produce(set<Attribute> *a) {
    for ( int i = 0; i < sub_conditions_count; i++){
        for (int j = 0; j < variables[i].size(); ++j) {
            a->insert(variables[i][j]);
        }
    }
    this->child->produce(a);
}

void HistSelectNode::consume(set<Attribute> *a) {
//    printf("%svector<int> %s_att[%i];\n", indent.c_str(), this->name.c_str(), sub_conditions_count);
//    printf("%srange<pair<int,int>> %s_range[%i];\n", indent.c_str(), this->name.c_str(), sub_conditions_count);
//    for (int i = 0; i < sub_conditions_count; ++i) {
//        for (int j = 0; j < variables[i].size(); ++j) {
//            printf("%s%s_att[%i].push_back(%s);\n", indent.c_str(), this->name.c_str(), i, variables[i][j].name.c_str());
//            printf("%s%s_range[%i].push_back(make_pair(%i,%i));\n", indent.c_str(), this->name.c_str(), i, ranges[i][j].first, ranges[i][j].second);
//        }
//    }cout << endl;

    cout << indent << "//APPLYING THE CONDITIONS OF EVERY SUBCONDITION INSIDE SELECT NODE\n\n";
    for (int k = 0; k < sub_conditions_count; ++k) {
        cout << indent << "{\n";
        indent += "\t";
        cout << indent << "//TAKING A COPY FROM EVERY ATTRIBUTE SO WE COULD RETRIEVE IT AFTER THIS SUBCONDITION EXECUTION\n\n";
        for (auto i = a->begin(); i != a->end(); i++) {
            string new_name = this->name + "_" + i->name;
            printf("%sint %s_bucket[BUCKET_COUNT];\n", indent.c_str(), new_name.c_str());
            printf("%sfor (int i = 0 ; i < BUCKET_COUNT; i++)\n", indent.c_str());
            printf("%s    %s_bucket[i] = %s_bucket[i];\n", indent.c_str(), new_name.c_str(), i->name.c_str());
            printf("%sint %s_min = %s_min;\n", indent.c_str(), new_name.c_str(), i->name.c_str());
            printf("%sint %s_max = %s_max;\n", indent.c_str(), new_name.c_str(), i->name.c_str());
            printf("%sint %s_table_size = table_size[%s];\n\n", indent.c_str(),
                   (this->name + "_" + i->table_name).c_str(), i->table_name.c_str());
        }
        cout << endl;
        cout << indent << "//APPLYING SUBCONDITIONS\n\n";
        for (int i = 0; i < variables[k].size(); ++i) {
            printf("%sdouble %s_ratio = limit(%s_bucket, %s_min, %s_max, %i, %i);\n",
                   indent.c_str(), variables[k][i].name.c_str(), variables[k][i].name.c_str(),
                   variables[k][i].name.c_str(),
                   variables[k][i].name.c_str(), ranges[k][i].first, ranges[k][i].second);
            for (auto j = a->begin(); j != a->end(); ++j) {
                if (variables[k][i].name != j->name && j->table_name == variables[k][i].table_name) {
                    printf("%sfor (int i = 0; i < BUCKET_COUNT; i++)\n"
                           "%s       %s_bucket[i] *= %s_ratio;\n",
                           indent.c_str(),
                           indent.c_str(), j->name.c_str(), variables[k][i].name.c_str());
                }
            }
            cout << endl;
        }
        cout << endl << endl;
        parent->consume(a);
        cout << indent << "//RETRIEVING ATTRIBUTES\n\n";
        for (auto i = a->begin(); i != a->end(); i++) {
            string new_name = this->name + "_" + i->name;
            printf("%sfor (int i = 0 ; i < BUCKET_COUNT; i++)\n", indent.c_str());
            printf("%s    %s_bucket[i] = %s_bucket[i];\n", indent.c_str(), i->name.c_str(), new_name.c_str());
            printf("%s%s_min = %s_min;\n", indent.c_str(), i->name.c_str(), new_name.c_str());
            printf("%s%s_max = %s_max;\n\n", indent.c_str(), i->name.c_str(), new_name.c_str());
//        printf("%s%s_table_size = table_size[%s];\n\n", indent.c_str(), (this->name + "_" + i->table_name).c_str(), i->table_name.c_str());
        }
        cout << endl;
        indent = indent.substr(0, indent.size() - 1);
        cout << indent << "}\n";
    }
}

void HistScanNode::produce(set<Attribute> *a) {
    cout << indent << "//SCANNING ATTRIBUTES(TAKING A COPY FROM DEFAULTS)\n\n";
    cout << indent;
    cout << "{\n";
    indent += "\t";
    for (auto i = a->begin(); i != a->end(); i++){
        printf("%sint %s_bucket[BUCKET_COUNT];\n", indent.c_str(), i->name.c_str());
        printf("%sfor (int i = 0 ; i < BUCKET_COUNT; i++)\n", indent.c_str());
        printf("%s    %s_bucket[i] = default_buckets[%s][%s][i];\n", indent.c_str(), i->name.c_str(), i->table_name.c_str(), i->name.c_str());
        printf("%sint %s_min = default_minimum[%s][%s];\n", indent.c_str(), i->name.c_str(), i->table_name.c_str(), i->name.c_str());
        printf("%sint %s_max = default_maximum[%s][%s];\n", indent.c_str(), i->name.c_str(), i->table_name.c_str(), i->name.c_str());
//        printf("%stable_size[%s] = default_table_size[%s];\n", indent.c_str(), i->table_name.c_str(), i->table_name.c_str());
        cout << endl;
//        printf("%sint %s[Max_buckets] = Histogram(&table_ints[%s], 25);\n", indent.c_str(), i->c_str(), i->c_str());
    } cout << endl;
    parent->consume(a);
}
void HistScanNode::consume(set<Attribute> *a) {}

void HistMapNode::produce(set<Attribute> *a) {
    a->insert(op1);
    a->insert(op2);
    auto in = indent.c_str();
    auto nm = name.c_str();
    printf("%sint %s_bucket[BUCKET_COUNT];\n"
           "%sint %s_min;\n"
           "%sint %s_max;\n",
           in, nm,
           in, nm,
           in, nm);
    this->child->produce(a);
}
void HistMapNode::consume(set<Attribute> *a) {
    string op1_name = op1.name;
    string op2_name = op2.name;
    auto in = indent.c_str();
    auto nm = name.c_str();
    auto o1 = op1_name.c_str();
    auto o2 = op2_name.c_str();

    if (type == map_sum){
        printf("%s//CALCULATING MAP EXPRESSION\n\n"
               "%s%s_min = %s_min + %s_min;\n"
               "%s%s_max = %s_max + %s_max;\n"
               "%sint bs1 = (%s_max - %s_min)/BUCKET_COUNT;\n"
               "%sint bs2 = (%s_max - %s_min)/BUCKET_COUNT;\n"
               "%sint bs3 = (%s_max - %s_min)/BUCKET_COUNT;\n"
               "%sfor (int i = 0; i < BUCKET_COUNT; ++i) {\n"
               "%s     for (int j = 0; j < BUCKET_COUNT; ++j) {\n"
               "%s          int val = (%s_min + i * bs1 + bs1/2) +\n"
               "%s               (%s_min + j * bs2 + bs2/2);\n"
               "%s          %s_bucket[(val - %s_min)/bs3] += %s_bucket[i] + %s_bucket[j];\n"
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
               in);
    }
    parent->consume(a);
}

void HistHistJoinNode::produce(set<Attribute> *a){
    a->insert(att1);
    a->insert(att2);
    this->child->produce(a);
}

void HistHistJoinNode::consume(set<Attribute> *a) {

    cout << indent << "//HIST-HIST JOIN\n";
    cout << indent << "{\n";

    indent += "\t";
    auto in = indent.c_str();
    auto nm = name.c_str();
    auto al = att1.name.c_str();
    auto ar = att2.name.c_str();

    printf("%sdouble expected_left_joins = 0;\n"
           "%sdouble expected_right_joins = 0;\n"
           "%sdouble bs = (%s_max - %s_min)/BUCKET_COUNT; //or (%s_max - %s_min)/BUCKET_SIZE\n "
           "%sfor (int i = 0; i < BUCKET_COUNT; i++){\n"
           "%s\tdouble bucket_left_joins = %s_bucket[i]/bs;\n"
           "%s\tdouble bucket_right_joins = %s_bucket[i]/bs;\n"
           "%s\texpected_left_joins += bucket_left_joins;\n"
           "%s\texpected_right_joins += bucket_right_joins;\n"
           "%s\t%s_bucket[i] *= bucket_left_joins;\n"
           "%s\t%s_bucket[i] *= bucket_right_joins;\n"
           "%s}\n"
           "%sexpected_left_joins /= BUCKET_COUNT;\n"
           "%sexpected_right_joins /= BUCKET_COUNT;\n\n",
    in,
    in,
    in, al, al, ar, ar,
    in,
    in, ar,
    in, al,
    in,
    in,
    in, al,
    in, ar,
    in,
    in,
    in);
    for (auto i = a->begin(); i != a->end() ; ++i) {
        if (i->table_name == att1.table_name) {
            printf("%sfor (int i = 0; i < BUCKET_COUNT; i++)\n"
                   "%s\t%s_bucket[i] *= expected_left_joins;\n\n",
                   in,
                   in, i->name.c_str());
        }
        if (i->table_name == att2.table_name) {
            printf("%sfor (int i = 0; i < BUCKET_COUNT; i++)\n"
                   "%s\t%s_bucket[i] *= expected_right_joins;\n\n",
                   in,
                   in, i->name.c_str());
        }
    }
    parent->consume(a);
    cout << endl;
    indent = indent.substr(0, indent.size() - 1);
    cout << indent << "}\n";
}

///////////////////////////////////////////////////////////////////////////////////////////////////