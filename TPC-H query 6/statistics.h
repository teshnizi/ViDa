//
// Created by teshnizi on 31/07/18.
// Contains statistical methods.
//

#ifndef CMAKE_BUILD_DEBUG_CMAKEFILES_VIDAG_STATISTICS_H
#define CMAKE_BUILD_DEBUG_CMAKEFILES_VIDAG_STATISTICS_H

#endif //CMAKE_BUILD_DEBUG_CMAKEFILES_VIDAG_STATISTICS_H

#include <vector>


//gets a sorted vector data and returns an approximate distribution function, as a histogram with "segment_count" segments:
template <typename T> vector <int> quantize(vector<T> data, int segment_count, float &block_size, float &lower_bound){
    lower_bound = data[data.size()/10];
    T upper_bound = data[9 * (data.size()/10)];
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
