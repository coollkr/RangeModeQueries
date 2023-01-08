/*
 * It is the file for the definition of the class used in this data structure
 */
#include <sdsl/bit_vectors.hpp>
#ifndef INTEGERATEDMETHOD_EXACTMODE_RRR_CDLMW_H
#define INTEGERATEDMETHOD_EXACTMODE_RRR_CDLMW_H

using namespace std;
using namespace sdsl;

class ExactMode_Subsr2_Compressed {
public:

    ExactMode_Subsr2_Compressed(int* array, int len, int s);
    void construct();
    pair<int, int> findMode(int start_index, int end_index);
    ~ExactMode_Subsr2_Compressed();


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



    rrr_vector<>** realBitArray;
    rrr_vector<>::select_0_type** select_0_array;
    rrr_vector<>::select_1_type** select_1_array;
    rrr_vector<>::rank_0_type** bit_rank_0_array;
    rrr_vector<>::rank_1_type** bit_rank_1_array;


    bit_vector* flag;
    rank_support_v5<0>* flag_rank_0;
    rank_support_v5<1>* flag_rank_1;




    pair<int, int> Third_Method(int start_index, int end_index);
    pair<int, int> bitMethod( int start_index, int end_index, int s, int t);
    int getCandidateFreq(int len, int freq_c, int x, int i, int j);
    int getSuffixCandidateFreq(int len, int freq_c, int x, int i, int j);
    void update_Ci_Table(int numOfBlocks);
    long long get1DSize (size_t byte, int row);
    long long get2DSize (size_t byte, int row, int col);

};

#endif //INTEGERATEDMETHOD_EXACTMODE_RRR_CDLMW_H
