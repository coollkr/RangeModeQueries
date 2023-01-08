/*
 * It is the file for the definition of the class used in this data structure
 */
#include <map>

#ifndef INC_77_EXACTMODE_CDLMW_H
#define INC_77_EXACTMODE_CDLMW_H

using namespace std;

class ExactMode_Sqrt{
public:
    ExactMode_Sqrt(int* array, int len, int s);
    void construct();
    pair<int, int> findMode(int start_index, int end_index);
    ~ExactMode_Sqrt();
private:
    int* array;
    int len;
    int s;
    int* A_Prime;
    int **Qa;
    int *QaSize;
    int **S;
    int **S_freq;
    int t;
    int delta;


    void getTheSTable();
    int getCandidateFreq(int freq_c, int x, int i, int j);
    int getSuffixCandidateFreq(int freq_c, int x, int i, int j);
    long long get1DSize (size_t byte, int row);
    long long get2DSize (size_t byte, int row, int col);
};



#endif //INC_77_EXACTMODE_CDLMW_H
