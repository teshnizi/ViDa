//
// Created by teshnizi on 31/07/18.
// Contains statistical methods.
//

#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

using namespace std;
//
//class Histogram{
//public:
//    Histogram(){};
//    Histogram(vector<int>* v, int bucket_count){
//        if (bucket_count == 0)
//            throw "segment count could not be 0";
//        if (v->size() == 0)
//            throw "array is empty";
//        bar.resize(bucket_count);
//        this->data_count = v->size();
//        this->data_sum = 0;
//        this->bucket_count = bucket_count;
//        minimum = (*v)[0];
//        maximum = (*v)[0];
//        for (int i = 0; i < v->size(); ++i) {
//            minimum = min(minimum, (*v)[i]);
//            maximum = max(maximum, (*v)[i]);
//            data_sum += (*v)[i];
//        }
//        mean = data_sum /= data_count;
//        bucket_size = ceil((float)(maximum + 1 - minimum)/(bucket_count));
//        for (int i = 0; i < v->size(); ++i) {
//            bar[((*v)[i]-minimum)/bucket_size]++;
//        }
//    }
//    string create_histogram_code(string name){
//        string s = "int " + name + "[Bar_count];\n";
//        s += "int " + name + "_min =  1e9;\n";
//        s += "int " + name + "_max = -1e9;\n";
//    }
//    //removes out of limit data from histogram
//    void limit(int l, int h){
//        for (int i = 0; i < bucket_count; ++i) {
//            int lo = minimum + bucket_size * i;
//            int hi = lo + bucket_size;
//            if (lo >= h || hi < l)
//                bar[i] = 0;
//            else if ( l <= lo && hi < h)
//                ;
//            else
//                bar[i] = bar[i] * ((lo < l) * (hi - l) + (lo >= l) * (h - lo))/(hi - lo);
//        }
//    }
//    Histogram operator+(const Histogram& h){
//        Histogram ans = Histogram();
//        ans.minimum = this->minimum + h.minimum;
//        ans.maximum = this->maximum + h.maximum;
//        ans.bucket_count = max(this->bucket_count, h.bucket_count);
//        ans.bucket_size = ceil((float)(ans.maximum + 1 - ans.minimum)/(ans.bucket_count));
//        ans.bar.resize(ans.bucket_count);
//        ans.data_count = 0;
//        for (int i = 0; i < this->bucket_count; ++i) {
//            for (int j = 0; j < h.bucket_count; ++j) {
//                ans.data_count += this->bar[i] * h.bar[j];
//                int val = (this->minimum + i*this->bucket_size + this->bucket_size/2) +
//                            (h.minimum + j*h.bucket_size + h.bucket_size/2);
//                ans.bar[(val - ans.minimum)/ans.bucket_size] += this->bar[i] + h.bar[j];
//            }
//        }
//        ans.data_sum = this->data_sum + h.data_sum;
//        ans.data_count = this->data_count + h.data_count;
//        ans.mean = (this->mean * this->data_count + h.mean * h.data_count)/ans.data_count;
//    }
//private:
//    vector<int> bar;
//    int bucket_count;
//    int minimum;
//    int maximum;
//    int bucket_size;
//    int data_sum;
//    int data_count;
//    int mean;
//};

//gets a sorted vector data and returns an approximate distribution function, as a histogram with "segment_count" segments:
vector <int>quantize(vector<int> data, int bucket_count, float &block_size, float &lower_bound){
    lower_bound = data[0];
    float upper_bound = data[data.size()-1];
//    if(upper_bound - lower_bound <= (bucket_count))
//        upper_bound = lower_bound + bucket_count;
    block_size = (upper_bound - lower_bound) / (bucket_count);
    vector <int> hist(bucket_count);
    for (int i = 0; i < data.size(); ++i) {
        int id = (int)(data[i]-lower_bound)/block_size;
        if ( id == bucket_count) id--;
        hist[id] += 1;
    }
    cout << lower_bound << " " << upper_bound << endl;
    return hist;
}

vector <string> string_generator(vector <string> table, vector <string> valid_strings){

    vector <int> upper_indices;
    vector <int> lower_indices;
    vector <int> ranges;
    vector <float> ratios;
    int table_size = table.size();

    for (int i = 0; i < valid_strings.size(); ++i) {
        upper_indices.push_back(upper_bound(table.begin(), table.end(), valid_strings[i]) - table.begin());
        lower_indices.push_back(lower_bound(table.begin(), table.end(), valid_strings[i]) - table.begin());
        ranges.push_back(upper_indices[i] - lower_indices[i]);
        ratios.push_back(ranges[i]/(float)table_size);
    }

    float output_size = table_size;
    for (int i = 0; i < ratios.size(); ++i)
        output_size *= ratios[i];
    for (int i = 1; i < ranges.size(); ++i)
        ranges[i] = ranges[i-1] + ranges[i];
}