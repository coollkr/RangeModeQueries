/*
 * It is the file for constructing the data structure and run the query.
 * The abbreviation for this data structure is simple,
 * and it is the data structure described in the following paper.
 *
 * Reference: Prosenjit Bose, Evangelos Kranakis, Pat Morin, and Yihui Tang. Approximate range mode and range median queries. In Annual Symposium on Theoretical Aspects of Computer Science, pages 377–388. Springer, 2005.
 *
 */

#define MEASURE_SPACE
#include <iostream>
#include <math.h>
#include "ApproxMode_Simple.h"

using namespace std;

ApproxMode_Simple :: ApproxMode_Simple(int* array, int len, double alpha) { // constructor
    this->array = array;
    this->len = len;
    this->alpha = alpha;
}
ApproxMode_Simple :: ~ApproxMode_Simple() { // destructor
    delete[] Ti;
    delete[] presumSize;
}
int ApproxMode_Simple :: getIndexValue(int index) { // return the value of array[index]
    return array[index];
}
int ApproxMode_Simple :: findQueryResult(int start_index, int end_index) { // return the mode, parameter: left index in the query and right index in the query
    int left = start_index - 1, right = end_index - 1;
    int finalMode = binarySearch(left, right);
    return finalMode;
}
int ApproxMode_Simple :: binarySearch(int start_index, int end_index) { // binary search on the table, return the mode, parameter: left index in the query and right index in the query
    int left = 0, right = presumSize[start_index + 1] - presumSize[start_index];
    if (presumSize[start_index + 1] - presumSize[start_index] == 0) {
        return array[start_index];
    }
    int front = presumSize[start_index];

    while (left < right) {  // keep find the result until two elements left
        int mid = left + (right - left) / 2;
        if (Ti[front + mid] <= end_index) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }

    if (left == 0) {
        return array[start_index];
    } else {
        return array[Ti[front + left - 1]];
    }
}
long long ApproxMode_Simple :: get1DSize (size_t byte, int row) { // // calculate the space usage of 1d array, return value is the space, para: size unit, the number of unit
    long long space = 0;
    space = byte * row;
    return space;
}
void ApproxMode_Simple :: construct() { // construct the data structure

    int delta = 0;
    for (int i = 0; i < len; i++) {
        delta = max(delta, array[i]);  // obtain the number of distinct values
    }

    int* Bsize = new int[len];
    presumSize = new int[len + 1];
    for (int i = 0; i < len; i++) {
        Bsize[i] = 0;
    }

#ifdef MEASURE_SPACE
    long long space = 0;
    space += get1DSize(sizeof(int), len + 1); // presumSize
#endif MEASURE_SPACE




    int limit = (log(len) / log(1 / alpha)) + 1;


    int rowNumber = limit;
    int** tmpTable = new int*[len];
    for (int i = 0; i < len; i++) {
        tmpTable[i] = new int[rowNumber];
    }
    // firstly build the tmpTable array to record, also record the valid elements in each row
    double prev = 1;
    int* count = new int[delta + 1];
    for (int r = 0; r < rowNumber; r++) { // update tmpTable row by row
        for (int q = 0; q < delta + 1; q++) {
            count[q] = 0;
        }
        int left = 0, right = 0;
        int fHigh = 1;
        if (r == 0) {
            fHigh = 1;
        } else {
            prev = prev * 1 / alpha;
            fHigh = ceil(prev);
        }
        for (right = 0; right < len; right++) {
            count[array[right]]++;
            if (count[array[right]] == fHigh) {
                tmpTable[left][r] = right;
                Bsize[left]++;
                count[array[left]]--;
                left++;
                while (left <= right && count[array[right]] == fHigh) {
                    tmpTable[left][r] = right;
                    Bsize[left]++;
                    count[array[left]]--;
                    left++;
                }
            }
        }
    }

    int numOfEntry = 0;
    presumSize[0] = 0;
    for (int i = 0; i < len; i++) {
        Bsize[i] = Bsize[i] - 1; // remove the first row
        numOfEntry += Bsize[i];
        if (i > 0) {
            presumSize[i] = presumSize[i - 1] + Bsize[i - 1];
        }
    }
    presumSize[len] = numOfEntry;


    // convert the element in tmpTable to the real Ti table

    Ti = new int[numOfEntry];

    int index = 0;
    for (int i = 0; i < len; i++) {
        for (int j = 0; j < Bsize[i]; j++) {
            Ti[index++] = tmpTable[i][j + 1];
        }
    }


#ifdef MEASURE_SPACE
    space += get1DSize(sizeof(int), numOfEntry);  //Ti
    cout << "space in bytes: " << space << endl;
#endif MEASURE_SPACE

    delete[] Bsize;
    for (int i = 0; i < len; i++) {
        delete[] tmpTable[i];
    }
    delete[] tmpTable;
    delete[] count;
}