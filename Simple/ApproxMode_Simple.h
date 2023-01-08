
/*
 * It is the file for the definition of the class used in this data structure
 */

#include <iostream>
using namespace std;
#ifndef PST_APPROXIMATEMODE_NV_BKMT_H
#define PST_APPROXIMATEMODE_NV_BKMT_H


class ApproxMode_Simple {
public:

    int getIndexValue(int index);
    ApproxMode_Simple(int *array, int len, double alpha);
    void construct();
    int findQueryResult(int start_index, int end_index);
    ~ApproxMode_Simple();


private:
    int* array;
    int* presumSize;
    int* Ti;

    int len;
    double alpha;
    int binarySearch(int start_index, int end_index);
    long long get1DSize (size_t byte, int row);
};
#endif //PST_APPROXIMATEMODE_BKMT_H
