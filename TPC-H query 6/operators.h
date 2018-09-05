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

enum attribute_virtuality{
    data_att,
    hist_att
};

const int condition_max_subconditions = 30;

struct Attribute{
    Attribute(string table_name, string name, int virtuality, int type){
        this->name = name;
        this->table_name = table_name;
        this->type = type;
        this->virtuality = virtuality;
    }
    string name;
    string table_name;
    int type;
    int virtuality;

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
class MapNode;
class JoinNode;

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

class ScanNode :public Node{
public:
    ScanNode(string name, Node* parent) : Node(name){
        this->parent = parent;
        parent->child = this;
    }
    void produce(set<Attribute> *a) override;
    void consume(set<Attribute> *a) override;
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
    void consume(set<Attribute> *a) override;

private:
    int int_sub_conditions;
    int string_sub_conditions;
    int brackets = 0;
    vector<Attribute> int_variables[condition_max_subconditions];
    vector<pair<int, int>> int_ranges[condition_max_subconditions];
    vector<Attribute> string_variables[condition_max_subconditions];
    vector<vector<string>> valid_strings[condition_max_subconditions];
};

class MapNode : public Node{
public:
    MapNode(string name, Node* parent, int type, int virtuality, Attribute op1, Attribute op2) : Node(name){
        this->parent = parent;
        this->parent->child = this;
        this->type = type;
        this->virtuality = virtuality;
        this->op1 = op1;
        this->op2 = op2;
    }
    void produce(set<Attribute> *a) override;
    void consume(set<Attribute> *a) override;

private:
    int type = map_sum;
    int virtuality = hist_att;
    Attribute op1 = Attribute("NULL", "NULL", att_int, data_att);
    Attribute op2 = Attribute("NULL", "NULL", att_int, data_att);
};

class JoinNode : public Node{
public:
    JoinNode(string name, Node* parent, Attribute att1, Attribute att2) : Node(name){
        this->att1 = att1;
        this->att2 = att2;
        this->parent = parent;
        this->parent->child = this;
    }
    void produce(set<Attribute> *a) override;
    void consume(set<Attribute> *a) override;

private:
    Attribute att1 = Attribute("NULL", "NULL", att_int, data_att);
    Attribute att2 = Attribute("NULL", "NULL", att_int, data_att);
};

///////////////////////////////////////////////////////////////////////////////////////////////////

void SelectNode::produce(set<Attribute> *a) {
    this->child->produce(a);
}

void SelectNode::consume(set<Attribute> *a) {
//    printf("%svector<int> %s_att[%i];\n", indent.c_str(), this->name.c_str(), int_sub_conditions);
//    printf("%srange<pair<int,int>> %s_range[%i];\n", indent.c_str(), this->name.c_str(), int_sub_conditions);
//    for (int i = 0; i < int_sub_conditions; ++i) {
//        for (int j = 0; j < int_variables[i].size(); ++j) {
//            printf("%s%s_att[%i].push_back(%s);\n", indent.c_str(), this->name.c_str(), i, int_variables[i][j].name.c_str());
//            printf("%s%s_range[%i].push_back(make_pair(%i,%i));\n", indent.c_str(), this->name.c_str(), i, int_ranges[i][j].first, int_ranges[i][j].second);
//        }
//    }cout << endl;

    cout << indent << "//APPLYING THE CONDITIONS OF EVERY SUBCONDITION INSIDE SELECT NODE\n\n";
    for (int k = 0; k < int_sub_conditions; ++k) {
        cout << indent << "{\n";
        indent += "\t";
        brackets++;

        cout << indent << "//TAKING A COPY FROM EVERY ATTRIBUTE SO WE COULD RETRIEVE IT AFTER THIS SUBCONDITION EXECUTION\n\n";
        for (auto i = a->begin(); i != a->end(); i++) {
            string new_name = this->name + "_" + i->name;
            if( i->virtuality == hist_att) {
                printf("%sint %s_bucket[BUCKET_COUNT];\n", indent.c_str(), new_name.c_str());
                printf("%sfor (int i = 0 ; i < BUCKET_COUNT; i++)\n", indent.c_str());
                printf("%s    %s_bucket[i] = %s_bucket[i];\n", indent.c_str(), new_name.c_str(), i->name.c_str());
                printf("%sint %s_min = %s_min;\n", indent.c_str(), new_name.c_str(), i->name.c_str());
                printf("%sint %s_max = %s_max;\n", indent.c_str(), new_name.c_str(), i->name.c_str());
                printf("%sint %s_table_size = table_size[%s];\n\n", indent.c_str(),
                       (this->name + "_" + i->table_name).c_str(), i->table_name.c_str());
            }
            if ( i->virtuality == data_att){
                string tp = "int";
                if(i->type == att_string)
                    tp = "string";
                printf("%s%s %s = %s;\n\n", indent.c_str(), tp.c_str(), new_name.c_str(), i->name.c_str());
            }
        }
        cout << endl;
        cout << indent << "//APPLYING SUBCONDITIONS\n\n";
        for (int i = 0; i < int_variables[k].size(); ++i) {
            if( int_variables[k][i].virtuality == hist_att) {
                printf("%sdouble %s_ratio = limit(%s_bucket, %s_min, %s_max, %i, %i);\n",
                       indent.c_str(), int_variables[k][i].name.c_str(), int_variables[k][i].name.c_str(),
                       int_variables[k][i].name.c_str(),
                       int_variables[k][i].name.c_str(), int_ranges[k][i].first, int_ranges[k][i].second);
                for (auto j = a->begin(); j != a->end(); ++j) {
                    if (int_variables[k][i].name != j->name && j->table_name == int_variables[k][i].table_name) {
                        printf("%sfor (int i = 0; i < BUCKET_COUNT; i++)\n"
                               "%s       %s_bucket[i] *= %s_ratio;\n",
                               indent.c_str(),
                               indent.c_str(), j->name.c_str(), int_variables[k][i].name.c_str());
                    }
                }
            }
            if ( int_variables[k][i].virtuality == data_att) {
                cout << indent;
                printf("if(%i < %s && %s < %i)\n", int_ranges[k][i].first, int_variables[k][i].name.c_str(), int_variables[k][i].name.c_str(), int_ranges[k][i].second);
                cout << indent <<"{\n";
                indent += "\t";
                brackets++;
            }
            cout << endl;
        }
        for (int i = 0; i < string_variables[k].size(); ++i) {
            if (string_variables[k][i].virtuality == hist_att) {
                printf("%sboolean %s_valids[BUCKET_COUNT] = {0};\n", indent.c_str(),
                       string_variables[k][i].name.c_str());
                for (int l = 0; l < valid_strings[k][i].size(); ++l) {
                    printf("%s%s_valids[%s] = 1;\n", indent.c_str(), string_variables[k][i].name.c_str(),
                           valid_strings[k][i][l].c_str());
                }
                printf("%sdouble %s_ratio = string_limit(%s_bucket, %s_valids);\n",
                       indent.c_str(), string_variables[k][i].name.c_str(), string_variables[k][i].name.c_str(),
                       string_variables[k][i].name.c_str());
                for (auto j = a->begin(); j != a->end(); ++j) {
                    if (string_variables[k][i].name != j->name && j->table_name == string_variables[k][i].table_name) {
                        printf("%sfor (int i = 0; i < BUCKET_COUNT; i++)\n"
                               "%s       %s_bucket[i] *= %s_ratio;\n",
                               indent.c_str(),
                               indent.c_str(), j->name.c_str(), string_variables[k][i].name.c_str());
                    }
                }
            }
            if ( string_variables[k][i].virtuality == data_att) {
                cout << indent << "if(\n";
                for (int l = 0; l < valid_strings[k][i].size(); ++l) {
                    printf("%s   (%s == %s)" , indent.c_str(), string_variables[k][i].name.c_str(), valid_strings[k][i][l].c_str());
                    if ( l != valid_strings[k][i].size() - 1)cout << "||";
                    cout << endl;
                }
                cout << indent <<"   ){\n";
                indent += "\t";
                brackets++;
            }
            cout << endl;
        }
        cout << endl << endl;
        parent->consume(a);

        //one remaining bracket is for condition itself
        while (brackets > 1) {
            indent = indent.substr(0, indent.size() - 1);
            cout << indent << "}\n";
            brackets--;
        }
        cout << indent << "//RETRIEVING ATTRIBUTES\n\n";
        for (auto i = a->begin(); i != a->end(); i++) {
            string new_name = this->name + "_" + i->name;
            if(i->virtuality == hist_att) {
                printf("%sfor (int i = 0 ; i < BUCKET_COUNT; i++)\n", indent.c_str());
                printf("%s    %s_bucket[i] = %s_bucket[i];\n", indent.c_str(), i->name.c_str(), new_name.c_str());
                printf("%s%s_min = %s_min;\n", indent.c_str(), i->name.c_str(), new_name.c_str());
                printf("%s%s_max = %s_max;\n\n", indent.c_str(), i->name.c_str(), new_name.c_str());
            }
            if(i->virtuality == data_att) {
                printf("%s%s = %s;\n\n", indent.c_str(), i->name.c_str(), new_name.c_str());
            }
        }
        cout << endl;
        indent = indent.substr(0, indent.size() - 1);
        cout << indent << "}\n";
        brackets--;
    }
}

void ScanNode::produce(set<Attribute> *a) {
    cout << indent << "//SCANNING ATTRIBUTES(TAKING A COPY FROM DEFAULTS)\n\n";
    cout << indent;
    cout << "{\n";
    indent += "\t";
    set<string> tables;
    for (auto i = a->begin(); i != a->end(); i++){
        if(i->virtuality == hist_att) {
            printf("%sint %s_bucket[BUCKET_COUNT];\n", indent.c_str(), i->name.c_str());
            printf("%sfor (int i = 0 ; i < BUCKET_COUNT; i++)\n", indent.c_str());
            printf("%s    %s_bucket[i] = default_buckets[%s][%s][i];\n", indent.c_str(), i->name.c_str(),
                   i->table_name.c_str(), i->name.c_str());
            printf("%sint %s_min = default_minimum[%s][%s];\n", indent.c_str(), i->name.c_str(), i->table_name.c_str(),
                   i->name.c_str());
            printf("%sint %s_max = default_maximum[%s][%s];\n", indent.c_str(), i->name.c_str(), i->table_name.c_str(),
                   i->name.c_str());
//        printf("%stable_size[%s] = default_table_size[%s];\n", indent.c_str(), i->table_name.c_str(), i->table_name.c_str());
            cout << endl;
//        printf("%sint %s[Max_buckets] = Histogram(&table_ints[%s], 25);\n", indent.c_str(), i->c_str(), i->c_str());
        }
        if ( i->virtuality == data_att){
            if(tables.find(i->table_name) == tables.end()) {
                tables.insert(i->table_name);
                cout << indent;
                printf("for (int %s_it = 0; %s_it < table_size[%s]; %s_it++)\n", i->table_name.c_str(),
                       i->table_name.c_str(), i->table_name.c_str(), i->table_name.c_str());
                cout << indent << "{\n";
                indent += "\t";
            }
            cout << indent;
            printf("%s = table[%s][%s_it];\n", i->name.c_str(), i->table_name.c_str(), (i->table_name.c_str()));
            cout << endl;
        }
    } cout << endl;
    parent->consume(a);
}

void ScanNode::consume(set<Attribute> *a) {}

void MapNode::produce(set<Attribute> *a) {
    a->insert(op1);
    a->insert(op2);
    auto in = indent.c_str();
    auto nm = name.c_str();
    if( virtuality == hist_att) {
        printf("%sint %s_bucket[BUCKET_COUNT];\n"
               "%sint %s_min;\n"
               "%sint %s_max;\n",
               in, nm,
               in, nm,
               in, nm);
    }
    if ( virtuality == data_att){
        printf("%sint %s = 0\n", in, nm);
    }
    this->child->produce(a);
}

void MapNode::consume(set<Attribute> *a) {
    string op1_name = op1.name;
    string op2_name = op2.name;
    auto in = indent.c_str();
    auto nm = name.c_str();
    auto o1 = op1_name.c_str();
    auto o2 = op2_name.c_str();

    if (type == map_sum) {
        if (virtuality == hist_att) {
            if (op1.virtuality == hist_att && op2.virtuality == hist_att) {
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
                printf("%s//CALCULATING MAP EXPRESSION\n\n"
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
                printf("%sERROR CALCULATING MAP EXPRESSION. CAN NOT CALCULATE A HIST FROM 2 NUMBERS\n\n");
            }
        }
        if (virtuality == data_att) {
            if (op1.virtuality == hist_att && op2.virtuality == hist_att) {
                printf("%s//CALCULATING MAP EXPRESSION\n\n"
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
                printf("%s//CALCULATING MAP EXPRESSION\n\n"
                       "%sint mean = 0;\n"
                       "%sint count = 0;\n"
                       "%sint bs = (%s_max - %s_min)/BUCKET_COUNT;\n"
                       "%sfor (int i = 0; i < BUCKET_COUNT; ++i) {\n"
                       "%s\tcount += %s_bucket[i];\n"
                       "%s\tmean += %s_bucket[i] * (%s_min + bs * i + bs/2);\n"
                       "%s\t}\n"
                       "%smean /= count;\n"
                       "%s%s = mean + %s;\n",
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
                printf("%s//CALCULATING MAP EXPRESSION\n\n"
                       "%s%s = %s + %s;\n",
                       in,
                       in, nm, o1, o2
                );
            }
        }
    }
    parent->consume(a);
}

void JoinNode::produce(set<Attribute> *a){
    a->insert(att1);
    a->insert(att2);
    this->child->produce(a);
}

void JoinNode::consume(set<Attribute> *a) {

    cout << indent << "//JOIN\n";
    cout << indent << "{\n";

    indent += "\t";
    auto in = indent.c_str();
    auto nm = name.c_str();
    auto al = att1.name.c_str();
    auto ar = att2.name.c_str();


    if ( att1.virtuality == hist_att && att2.virtuality == hist_att) {
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
        for (auto i = a->begin(); i != a->end(); ++i) {
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
    }

    if ((att1.virtuality == hist_att && att2.virtuality == data_att) ||
        (att2.virtuality == hist_att && att1.virtuality == data_att)) {
        if (att1.virtuality == data_att) {
            string tt = al;
            al = ar;
            ar = tt.c_str();
        }
        printf("%sdouble ratio = 1;\n"
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
                printf("%sfor (int i = 0; i < BUCKET_COUNT; i++)\n"
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
        indent += "\t";
        printf("%sif (%s == %s){\n\n", in, al, ar);
    }
    parent->consume(a);
    cout << endl;
    if (bracket)
        indent = indent.substr(0, indent.size() - 1);
    //this one is for join itself:
    indent = indent.substr(0, indent.size() - 1);
    cout << indent << "}\n";
}

///////////////////////////////////////////////////////////////////////////////////////////////////