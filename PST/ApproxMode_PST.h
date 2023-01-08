
/*
 * It is the file for the definition of the class used in this data structure
 */

#include "adds.h"
#ifndef PST_APPROXIMATEMODE_BKMT_H
#define PST_APPROXIMATEMODE_BKMT_H


class ApproxMode_PST {
public:

    ApproxMode_PST(int *array, int len, double alpha);
    void construct();
    int findQueryResult(int start_index, int end_index);
    ~ApproxMode_PST();
    int getIndexValue(int index);
private:
    int* array;
    int len;
    double alpha;
    PTree *tree;

};
#endif //PST_APPROXIMATEMODE_BKMT_H
