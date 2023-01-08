/*
 * It is the file for the definition of the class used in this data structure
 */
#include <map>
#include <sdsl/bit_vectors.hpp>


#ifndef HEBITPLUS_APPROXIMATEMODE_BIT_EHMNS_H
#define HEBITPLUS_APPROXIMATEMODE_BIT_EHMNS_H

using namespace std;
using namespace sdsl;

class ApproxMode_Succ_Plain{
public:
    ApproxMode_Succ_Plain(int* array, int len, double epsilon);
    void construct();
    int findQueryResult(int start_index, int end_index);
    ~ApproxMode_Succ_Plain();
    int getIndexValue(int index);

private:
    int* array;
    double epsilon;
    int len;



    int Qk_len;
    bit_vector **Qk;

    select_support_mcl<0>** Qk_select;
    rank_support_v5<1>** Qk_rank_1;
    int* numberOfZeroInQk;

    bit_vector*** Kr;

    select_support_mcl<1> ***Kr_select;
    rank_support_v5<0> ***Kr_rank_0;


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
