//
// Created by teshnizi on 10/10/18.
//

#ifndef CMAKE_BUILD_DEBUG_CMAKEFILES_VIDAG_EXPRESSIONS_H
#define CMAKE_BUILD_DEBUG_CMAKEFILES_VIDAG_EXPRESSIONS_H

#endif //CMAKE_BUILD_DEBUG_CMAKEFILES_VIDAG_EXPRESSIONS_H


#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

enum attribute_type{
    att_int,
    att_string
};

enum attribute_virtuality{
    data_att,
    hist_att,
    dist_att
};

struct Attribute{
    Attribute(string table_name, string name, int virtuality, int type, int id){
        this->name = name;
        this->table_name = table_name;
        this->type = type;
        this->virtuality = virtuality;
        this->id = id;
    }
    Attribute(string table_name, string name, int virtuality, string distribution, int type, int id)
            : Attribute(table_name, name, virtuality, type, id){
        this->distribution = distribution;
    }
    string name;
    string table_name;
    int type;
    int virtuality;
    int id;
    string distribution;

    bool operator < (const Attribute& a) const{
        if(this->table_name == a.table_name){
            return this->name < a.name;
        }
        return this->table_name < a.table_name;
    }
    bool operator == (const Attribute& a) const{
        return (this->table_name == a.table_name) && (this->table_name == a.table_name);
    }
};

struct Histogram{
    float bucket[100];
    float minimum;
    float maximum;
    int bucket_count = -1;
    float bucket_size;
};

typedef struct Histogram Histogram;

struct DataItem{
    int data;
    int key;
};

typedef struct DataItem DataItem;


double hist_hist_sum(Histogram a, Histogram b){
    double val1 = 0;
    double val2 = 0;
    double count1 = 0;
    double count2 = 0;
    double summation = 0;
    double bs1 = (a.maximum - a.minimum)/a.bucket_count;
    double bs2 = (b.maximum - b.minimum)/b.bucket_count;
    for (int i = 0; i < a.bucket_count; ++i) {
        count1 += a.bucket[i];
    }
    for (int i = 0; i < b.bucket_count; ++i) {
        count2 += b.bucket[i];
    }
    double mlt = count1 * count2;
    for (int i = 0; i < a.bucket_count; ++i) {
        for (int j = 0; j < b.bucket_count; ++j) {
            val1 = a.bucket[i] * (a.minimum + bs1 * i + bs1/2);
            val2 = b.bucket[j] * (b.minimum + bs2 * j + bs2/2);
            summation += (val1 * val2) / mlt;
        }
    }
    return summation;
}

double data_hist_sum(double x, Histogram h){
    double mean = 0;
    double count = 0;
    double bs = (h.maximum - h.minimum)/h.bucket_count;
    for (int i = 0; i < h.bucket_count; ++i) {
        count += h.bucket[i];
        mean += h.bucket[i] * (h.minimum + bs * i + bs/2);
    }
    mean /= count;
    return mean * x;
}

class ExpressionNode{
public:
    ExpressionNode(ExpressionNode* left, ExpressionNode* right){
        this->left = left;
        this->right = right;
    }
    virtual string get_type(){};
    virtual string run(){};
    virtual set<Attribute> scan_atts(){};
//protected:
    ExpressionNode* left;
    ExpressionNode* right;

};

class OperatorNode : public ExpressionNode{
public:
    OperatorNode(ExpressionNode* left, ExpressionNode* right, bool is_unary, string func_name, bool has_hist_output) : ExpressionNode(left, right){
        this->is_unary = is_unary;
        this->has_hist_output = has_hist_output;
        this->func_name = func_name;
    }

    string get_type(){
        if (has_hist_output)
            return "hist";
        else
            return "data";
    }

    string run(){
        if (is_unary){
            return "(" + func_name + left->run() + ")";
        }
        return left->get_type() + "_" + right->get_type() + "_" + func_name + "_" + this->get_type()
               + "(" + left->run() + ", " + right->run() + ")";
    }

    set<Attribute> scan_atts(){
        set<Attribute> s1 = left->scan_atts();
        set<Attribute> s2 = right->scan_atts();
        set<Attribute> ret;
        for (auto i = s1.begin(); i != s1.end() ; ++i) {
            ret.insert(*i);
        }
        for (auto i = s2.begin(); i != s2.end() ; ++i) {
            ret.insert(*i);
        }
        return ret;
    }

private:
    bool is_unary;
    bool has_hist_output;
    string func_name;
};


class OperandNode : public ExpressionNode{
public:
    OperandNode(Attribute* att) : ExpressionNode(NULL, NULL){
        this->att = att;
        this->has_att = true;
    }
    OperandNode(string numeric) : ExpressionNode(NULL, NULL){
        this->numeric = numeric;
        this->has_att = false;
    }

    string get_type(){
        if (has_att && att->virtuality == hist_att)
            return "hist";
        else
            return "data";
    }
    string run(){
        if (has_att) {
            if (att->virtuality == hist_att)
                return att->name + "_hist";
            else
                return att->name;
        }
        else
            return numeric;
    }

    set<Attribute> scan_atts(){
        set<Attribute> s;
        if(has_att)
            s.insert(*att);
        return s;
    }

    string num(){
        return numeric;
    }
private:
    bool has_att = false;
    Attribute* att;
    string numeric;
};