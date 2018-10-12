//
// Created by teshnizi on 31/07/18.
// Contains statistical methods.
//

#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

using namespace std;

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