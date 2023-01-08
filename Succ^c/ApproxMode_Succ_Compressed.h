/*
 * It is the file for the definition of the class used in this data structure
 */
#include <map>
#include <sdsl/bit_vectors.hpp>


#ifndef HEBITPLUS_APPROXIMATEMODE_BIT_EHMNS_H
#define HEBITPLUS_APPROXIMATEMODE_BIT_EHMNS_H

using namespace std;
using namespace sdsl;

class ApproxMode_Succ_Compressed{
public:
    ApproxMode_Succ_Compressed(int* array, int len, double epsilon);
    void construct();
    int findQueryResult(int start_index, int end_index);
    ~ApproxMode_Succ_Compressed();
    int getIndexValue(int index);

    int largerQuery;

private:
    int* array;
    double epsilon;
    int len;



    int Qk_len;
    rrr_vector<> **Qk;

    rrr_vector<>::select_0_type** Qk_select;
    rrr_vector<>::rank_1_type** Qk_rank_1;
    int* numberOfZeroInQk;

    rrr_vector<>*** Kr;

    rrr_vector<>::select_1_type ***Kr_select;
    rrr_vector<>::rank_0_type ***Kr_rank_0;


    int** numberOfOneInKr;

    int* ceil_f_2k_minus_1Array;
    int* ceil_f_2kArray;

    int k_upper_bound;
    int delta;




    int findLargeMode(int start, int end);
    int getRi(int curK, int i);
    int getRj(int curK, int j);
    int findSi(int start,  int k);
    int findSj(int start,  int k);

    int findLowFreq(int start, int end);
    int getQki(int k, int i);

};
#endif //HEBITPLUS_APPROXIMATEMODE_BIT_EHMNS_H
