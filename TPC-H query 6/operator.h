
#include <iostream>
#include <bitset>
#include <vector>
#include "tuples2.h"
#include <set>


using namespace std;

enum aggregation_type{
    nothing= 0,
    agg_sum,
    agg_mean
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

string after_print = "";

string current_indent = "";
int brackets = 0;

class Node;
class DataSelectNode;
class DataScanNode;

class Node{
public:
    virtual void consume(set<string> a, Node* source){
        while(brackets--) {
            current_indent = current_indent.substr(0, current_indent.size()-1);
            cout << current_indent << "}\n";
        }
    };
    virtual void produce(set<string> a){};
    void set_child(Node* child){
        this->child = child;
    }
    Node* child;
    Node* parent;
};

class DataSelectNode : public Node{
public:
    DataSelectNode(Node* parent, vector<string> variables, vector<pair<int,int>> ranges){
        this->variables = variables;
        this->ranges = ranges;
        this->parent = parent;
    }
    void produce(set<string> a);
    void consume(set<string> a, Node* source);
    vector<string> variables;
    vector<pair<int, int>> ranges;
};

class DataScanNode : public Node{
public:
    DataScanNode(Node* parent, string name, int sz){
        this->sz = sz;
        this->name = name;
        this->parent = parent;
    }
    void produce(set<string> a);
    void consume(set<string> a, Node* source);
    int sz;
    string name;
};

class DataMapNode : public Node{
public:
    DataMapNode(Node* parent, string expression, string ans_name){
        if(expression.substr(0,3) == "sum"){
            agg_type = agg_sum;
            expression = expression.substr(3,expression.size());
        }
        else if(expression.substr(0,4) == "mean"){
            agg_type = agg_mean;
            expression = expression.substr(4,expression.size());
        } else {
            agg_type = nothing;
        }

        this->expression = expression;
        this->parent = parent;
        this->ans_name = ans_name;
    }
    void produce(set<string> a);
    void consume(set<string> a, Node* source);
    string expression;
    string ans_name;
    bool agg_type;
};

///////////////////////////////////////////////////////////////////////////////////////////////////

void DataSelectNode::produce(set<string> a) {
    for (int i = 0; i < variables.size(); ++i) {
        a.insert(variables[i]);
    }
    this->child->produce(a);
}
void DataSelectNode::consume(set<string> a, Node* source) {
    for (int i = 0; i < variables.size(); ++i) {
        cout << current_indent;
        printf("if(%i < %s && %s < %i)\n", ranges[i].first, variables[i].c_str(), variables[i].c_str(), ranges[i].second);
    }
    cout << current_indent <<"{\n";
    current_indent += "\t";
    brackets++;
    parent->consume(a, this);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void DataScanNode::produce(set<string> a) {
    cout << current_indent;
    printf("for (int %s_it = 0; %s_it < %d; %s_it++)\n", name.c_str(), name.c_str(), sz, name.c_str());
    cout << "{\n";
    current_indent += "\t";
    for (auto i = a.begin(); i != a.end(); ++i) {
        cout << current_indent;
        printf("%s = table[l_%s][%s_it];\n", (*i).c_str(), (*i).c_str(), (this->name).c_str());
    }cout << endl;
    brackets++;
    this->parent->consume(a, this);
}
void DataScanNode::consume(set<string> a, Node* source) {}

///////////////////////////////////////////////////////////////////////////////////////////////////

void DataMapNode::produce(set<string> a) {
    vector<string> v = expression_parser(this->expression);
    for (int i = 0; i < v.size(); ++i) {
        a.insert(v[i]);
    }
    cout << current_indent;
    printf("int %s = 0;\n", this->ans_name.c_str());
    this->child->produce(a);
}
void DataMapNode::consume(set<string> a, Node *source) {
    char ch=' ';
    if ( this->agg_type != nothing)ch = '+';
    printf("%s%s %c= %s;\n", current_indent.c_str(), (this->ans_name).c_str(), ch, (this->expression).c_str());
    a.insert(this->ans_name);
    this->parent->consume(a, this);
}
