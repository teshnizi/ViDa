
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

string indent = "";
int brackets = 0;

class Node;
class DataScanNode;
class DataSelectNode;
class MapNode;
class HistScanNode;
class HistSelectNode;


class Node{
public:
    virtual void consume(set<string> a, Node* source){
        while(indent.size() > 0) {
            indent = indent.substr(0, indent.size()-1);
            cout << indent << "}\n";
        }
    };
    virtual void produce(set<string> a){
        this->child->produce(a);
    };
    void set_child(Node* child){
        this->child = child;
    }
    Node* child;
    Node* parent;
};

class DataScanNode : public Node{
public:
    DataScanNode(Node* parent, string name, int sz){
        this->sz = sz;
        this->name = name;
        this->parent = parent;
    }
    void produce(set<string> a) override ;
    void consume(set<string> a, Node* source) override ;

private:
    int sz;
    string name;
};
class DataSelectNode : public Node{
public:
    DataSelectNode(Node* parent, vector<string> variables, vector<pair<int,int>> ranges){
        this->variables = variables;
        this->ranges = ranges;
        this->parent = parent;
    }
    void produce(set<string> a) override;
    void consume(set<string> a, Node* source) override;

private:
    vector<string> variables;
    vector<pair<int, int>> ranges;
};
class MapNode : public Node{
public:
    MapNode(Node* parent, string expression, string ans_name){
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
    void produce(set<string> a) override;
    void consume(set<string> a, Node* source) override;
private:
    string expression;
    string ans_name;
    bool agg_type;
};
class DataAggregateNode : public Node{
public:
    DataAggregateNode(string name, vector<string>* aggregate_atts, Node* parent){
        this->parent = parent;
        this->name = name;
        this->aggregate_atts = aggregate_atts;
    }
    void produce(set<string> a) override;
    void consume(set<string> a, Node* source) override;

private:
    string name;
    vector<string> *aggregate_atts;
};


class HistScanNode :public Node{
public:
    HistScanNode(Node* parent){
        this->parent = parent;
    }
    void produce(set<string> a) override ;
    void consume(set<string> a, Node* source) override ;
};
class HistSelectNode : public Node{
public:
    HistSelectNode(Node* parent, vector<string> variables, vector<pair<int,int>> ranges){
        this->variables = variables;
        this->ranges = ranges;
        this->parent = parent;
    }
    void produce(set<string> a) override ;
    void consume(set<string> a, Node* source) override ;

private:
    vector<string> variables;
    vector<pair<int, int>> ranges;
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
        cout << indent;
        printf("if(%i < %s && %s < %i)\n", ranges[i].first, variables[i].c_str(), variables[i].c_str(), ranges[i].second);
    }
    cout << indent <<"{\n";
    indent += "\t";
    brackets++;
    parent->consume(a, this);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void DataScanNode::produce(set<string> a) {
    cout << indent;
    printf("for (int %s_it = 0; %s_it < %d; %s_it++)\n", name.c_str(), name.c_str(), sz, name.c_str());
    cout << "{\n";
    indent += "\t";
    for (auto i = a.begin(); i != a.end(); ++i) {
        cout << indent;
        printf("%s = table[l_%s][%s_it];\n", (*i).c_str(), (*i).c_str(), (this->name).c_str());
    }cout << endl;
    brackets++;
    this->parent->consume(a, this);
}
void DataScanNode::consume(set<string> a, Node* source) {}

///////////////////////////////////////////////////////////////////////////////////////////////////

void MapNode::produce(set<string> a) {
    vector<string> v = expression_parser(this->expression);
    for (int i = 0; i < v.size(); ++i) {
        a.insert(v[i]);
    }
    cout << indent;
    printf("int %s = 0;\n", this->ans_name.c_str());
    this->child->produce(a);
}
void MapNode::consume(set<string> a, Node *source) {
    char ch=' ';
    if ( this->agg_type != nothing)ch = '+';
    printf("%s%s %c= %s;\n", indent.c_str(), (this->ans_name).c_str(), ch, (this->expression).c_str());
    a.insert(this->ans_name);
    this->parent->consume(a, this);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void DataAggregateNode::produce(set<string> a) {
    cout<<indent << "materialize(" << name << ")\n";
}
void DataAggregateNode::consume(set<string> a, Node *source) {

}

///////////////////////////////////////////////////////////////////////////////////////////////////

void HistScanNode::consume(set<string> a, Node *source) {
    this->parent->consume(a, this);
}
void HistSelectNode::produce(set<string> a) {
    for (int i = 0; i < variables.size(); ++i) {
        a.insert(variables[i]);
    }
    this->child->produce(a);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void HistScanNode::produce(set<string> a) {
    cout << indent;
    cout << "{\n";
    indent += "\t";
    for (auto i = a.begin(); i != a.end(); i++){
        printf("%sHistogram %s = Histogram(&table_ints[%s], 25);\n", indent.c_str(), i->c_str(), i->c_str());
    } cout << endl;
    this->parent->consume(a, this);
}
void HistSelectNode::consume(set<string> a, Node *source) {
    for (int i = 0; i < variables.size(); ++i) {
        printf("%s%s.limit(%i, %i);\n", indent.c_str(), variables[i].c_str(), ranges[i].first, ranges[i].second);
    }cout << endl;
    this->parent->consume(a, this);
}

///////////////////////////////////////////////////////////////////////////////////////////////////