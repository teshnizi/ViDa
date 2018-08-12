//
// Created by teshnizi on 31/07/18.
// Contains statistical methods.
//

#ifndef CMAKE_BUILD_DEBUG_CMAKEFILES_VIDAG_STATISTICS_H
#define CMAKE_BUILD_DEBUG_CMAKEFILES_VIDAG_STATISTICS_H

#endif //CMAKE_BUILD_DEBUG_CMAKEFILES_VIDAG_STATISTICS_H

#include <vector>


//gets a sorted vector data and returns an approximate distribution function, as a histogram with "segment_count" segments:
vector <int> quantize(vector<int> data, int segment_count, int &block_size, int &lower_bound){
    lower_bound = data[data.size()/10];
    int upper_bound = data[9 * (data.size()/10)];
    if(upper_bound - lower_bound <= (segment_count - 2))
        upper_bound = lower_bound + segment_count - 2;
    block_size = (upper_bound - lower_bound) / (segment_count - 2);
    vector <int> hist(segment_count);

    for (int i = 0; i < data.size(); ++i) {

        if( data[i] < lower_bound )
            hist[0]++;
        else if( data[i] > upper_bound )
            hist[segment_count - 1]++;
        else hist[(data[i]-lower_bound)/block_size] += 1;
    }
    lower_bound -= block_size;
    return hist;
}


float get_random_data_by_hist(vector<int> cml_hist, float block_size, float lower_bnd){

    int k = rand() % cml_hist[cml_hist.size()-1];
    k = lower_bound(cml_hist.begin(), cml_hist.end(), k) - cml_hist.begin();
    return ((rand()/(float)RAND_MAX) * block_size + (float)k * block_size + lower_bnd);
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
