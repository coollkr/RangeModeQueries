
/*
 * It is the file for the definition of the class used in this data structure
 */

#include <map>


#ifndef INC_77_EXACTMODE_CDLMW_H
#define INC_77_EXACTMODE_CDLMW_H

using namespace std;

class ExactMode_Supsr{
public:
    ExactMode_Supsr(int* array, int len, int s);
    void construct();
    pair<int, int> findMode(int start_index, int end_index);
    ~ExactMode_Supsr();
private:
    int* array;
    int len;
    int s;

    int **Qa;
    int *QaSize;
    int **S;

    int t;
    int delta;


    void getTheSTable();
    long long get1DSize (size_t byte, int row);
    long long get2DSize (size_t byte, int row, int col);
    int findLeftBound(int cur, int startIndex);
    int findRightBound(int cur, int endIndex);
};



#endif //INC_77_EXACTMODE_CDLMW_H
