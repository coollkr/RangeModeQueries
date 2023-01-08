
/*
 * It is the file for the definition of the class used in this data structure
 */
#include <map>
#include <sdsl/bit_vectors.hpp>


#ifndef INC_1BITPLUS_APPROXIMATEMODE_BIT_GJLT_H
#define INC_1BITPLUS_APPROXIMATEMODE_BIT_GJLT_H

using namespace std;
using namespace sdsl;

class ApproxMode_Sample_Plain{
public:
    ApproxMode_Sample_Plain(int* array, int len, double epsilon);
    int construct();
    int findQueryResult(int start_index, int end_index);
    ~ApproxMode_Sample_Plain();


private:
    int* array;
    double epsilon;
    int len;
    int endK;
    unsigned int* Qi;
    long Qi_len;

    int* TiSize;
    unsigned int** presumBitSize;
    int* DlSize;
    int* SlSize;
    int** bitSize;
    int** ClSize;

    unsigned int** presumClSize;

    int delta;
    pair<int, int> ** Ti;
    long Ti_len;

    pair<unsigned int, unsigned int>** Cl;
    int** Dl;
    pair<int, int> **El;
    pair<int, int>** Sl;
    int t;
    int Dl_start_len;
    long I_len;

    bit_vector*** tmpbitArray;
    bit_vector **bitArray;
    rank_support_v5<1>**  newRank;
    int *saveK;
    int *largeK;
    int* I;


    int Dl_len;
    int k_front_half_bound;




    void getSaveK();
    void getLargeK();
    int resultBinarySearch(int left, int right);
    pair<int, int> findTi(int i, int k);
    int successor_search(int i, int j);
    long long get1DSize (size_t byte, long row);
    long long get2DSize (size_t byte, long row, long col);
};
#endif //INC_1BITPLUS_APPROXIMATEMODE_BIT_GJLT_H
