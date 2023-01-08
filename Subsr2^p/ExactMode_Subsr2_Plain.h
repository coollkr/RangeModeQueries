
/*
 * It is the file for the definition of the class used in this data structure
 */
#include <sdsl/bit_vectors.hpp>

#ifndef TIMOTHYBITVECTOR_EXACTMODE_BIT_CDLMW_H
#define TIMOTHYBITVECTOR_EXACTMODE_BIT_CDLMW_H


using namespace std;
using namespace sdsl;

class ExactMode_Subsr2_Plain {
public:

    ExactMode_Subsr2_Plain(int* array, int len, int s);
    void construct();
    pair<int, int> findMode(int start_index, int end_index);
    ~ExactMode_Subsr2_Plain();

private:
    int* array;
    int len;
    int s;

    int t;
    int B0_len;
    int B1_len;


    int delta_B0;
    int delta_B1;

    int **Qa_B1;
    int blocks;

    int **Ci;



    int* B0;
    int* B1;
    int* B1_Prime;


    int numOfBlocks;
    int* QaSize;

    int** M;

    bit_vector** realBitArray;
    select_support_mcl<0>** select_0_array;
    select_support_mcl<1>** select_1_array;
    rank_support_v5<0>** bit_rank_0_array;
    rank_support_v5<1>** bit_rank_1_array;



    bit_vector* flag;
    rank_support_v5<0>* flag_rank_0;
    rank_support_v5<1>* flag_rank_1;


    pair<int, int> Third_Method(int start_index, int end_index);
    pair<int, int> bitMethod( int start_index, int end_index, int s, int t);
    int getCandidateFreq( int len, int freq_c, int x, int i, int j);
    int getSuffixCandidateFreq(  int len, int freq_c, int x, int i, int j);
    void update_Ci_Table(int numOfBlocks);
    long long get1DSize (size_t byte, int row);
    long long get2DSize (size_t byte, int row, int col);

};

#endif //TIMOTHYBITVECTOR_EXACTMODE_BIT_CDLMW_H
