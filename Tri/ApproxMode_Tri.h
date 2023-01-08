/*
 * It is the file for the definition of the class used in this data structure
 */
#include <map>



#ifndef HEBITPLUS_APPROXIMATEMODE_BIT_EHMNS_H
#define HEBITPLUS_APPROXIMATEMODE_BIT_EHMNS_H

using namespace std;


class ApproxMode_Tri{
public:
    ApproxMode_Tri(int* array, int len, double epsilon);
    void construct();
    int findQueryResult(int start_index, int end_index);
    ~ApproxMode_Tri();
    int getIndexValue(int index);

private:
    int* array;
    double epsilon;
    int len;



    int Qk_len;
    int** Qi;

    int** riArray;
    int** rjArray;
    int* riSize;
    int* rjSize;


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
