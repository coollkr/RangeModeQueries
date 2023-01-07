/*
 * It is the file for constructing the data structure and run the query.
 * The abbreviation for this data structure is supsr,
 * and it is the data structure described in the following paper.
 *
 * Reference:
 * Danny Krizanc, Pat Morin, and Michiel H. M. Smid. Range mode and range median queries on lists and trees. Nordic Journal of Computing, 12(1):1–17, 2005.
 */

#define MEASURE_SPACE

#include <iostream>
#include <fstream>
#include "ExactMode_Supsr.h"

using namespace std;


ExactMode_Supsr :: ExactMode_Supsr(int* array, int len, int s) { // constructor
    this->array = array;
    this->len = len;
    this->s = s;

}
ExactMode_Supsr :: ~ExactMode_Supsr() { // destructor
    delete[] QaSize;
    for (int i = 0; i < delta; i++) {
        delete[] Qa[i];
    }
    delete[] Qa;
    for (int i = 0; i < s; i++) {
        delete[] S[i];
    }
    delete[] S;
}
void ExactMode_Supsr :: construct() { // construct the data structure


#ifdef MEASURE_SPACE
    long long space = 0;
    space += get1DSize(sizeof(int), len);  // input array
#endif MEASURE_SPACE

    delta = 0;
    for (int i = 0; i < len; i++) {  // get the number of distinct value
        delta = max(delta, array[i]);
    }





    Qa = new int*[delta];            // initialize 2-D dynamic array Qa
    QaSize = new int[delta];


    for (int i = 0; i < delta; i++) {
        QaSize[i] = 0;
    }

    for (int i = 0; i < len; i++) {  // update A_Prime
        QaSize[array[i]-1]++;
    }


#ifdef MEASURE_SPACE
    int total = 0;
#endif MEASURE_SPACE





    for (int i = 0; i < delta; i++) {
        Qa[i] = new int[QaSize[i]];
#ifdef MEASURE_SPACE
        total += QaSize[i];
#endif MEASURE_SPACE
    }
    int* index = new int[delta + 1];
    for (int i = 0; i < delta + 1; i++) {
        index[i] = 0;
    }
    for (int i = 0; i < len; i++) {
        int cur = array[i];
        Qa[cur - 1][index[cur]++] = i + 1;
    }


#ifdef MEASURE_SPACE
    space += get1DSize(sizeof(int), delta);  //QaSize
    space += get1DSize(sizeof(int), total); // Qa
    space += get1DSize(sizeof(int*), delta); // Qa pointer
#endif MEASURE_SPACE


    delete[] index;


    t = (len + s - 1) / (s); // t: the size of every block, s : the fixed number of blocks




    S = new int*[s];

    for (int i = 0; i < s; i++) {
        S[i] = new int[s];
    }



    getTheSTable();  // create the table S and S_freq

#ifdef MEASURE_SPACE
    space += get2DSize(sizeof(int), s, s);  // S space
    space += get1DSize(sizeof(int*), s);  // S pointer
    cout << "space in bytes: " << space << endl;
    return;
#endif MEASURE_SPACE

}
long long ExactMode_Supsr :: get1DSize (size_t byte, int row) { // calculate the space usage of 1d array, return value is the space, para: size unit, the number of unit
    long long space = 0;
    space = byte * row;
    return space;
}
long long ExactMode_Supsr :: get2DSize (size_t byte, int row, int col) { // calculate the space usage of 2d array, return value is the space, para: size unit, the number of unit
    long long space = 0;
    space = byte * row * col;
    return space;
}

pair<int, int> ExactMode_Supsr :: findMode(int start_index, int end_index) {  // find query result, return value: pair<mode, frequency>, para: left index of query and right index of query
    pair<int, int> res;
    int bi = ((start_index-1) + t - 1) / t, bj = (end_index/t) - 1;
    int prefix_start = start_index;
    int prefix_end = min(bi * t, end_index);
    int suffix_start = max((bj + 1) * t + 1, start_index);
    int suffix_end = end_index;
    int c = 0;  // candidate
    int freq_c = 0;

    if (bi <= bj) {    // not in one span
        int span_mode = S[bi][bj];
        c = span_mode;  // candidate
        int k = findLeftBound(span_mode, start_index);
        int l = findRightBound(span_mode, end_index);
        freq_c = l - k + 1;

        for (int x = prefix_start; x <= prefix_end; x++) {       // start to traverse the prefix to find the candidate
            int k = findLeftBound(array[x - 1], start_index);
            int l = findRightBound(array[x - 1], end_index);
            int freq = l - k + 1;
            if (freq > freq_c) {
                freq_c = freq;
                c = array[x];
            }
        }// the end of traversing the prefix
        for (int x = suffix_start; x <= suffix_end; x++) {       // start to traverse the suffix to find the candidate
            int k = findLeftBound(array[x - 1], start_index);
            int l = findRightBound(array[x - 1], end_index);
            int freq = l - k + 1;
            if (freq > freq_c) {
                freq_c = freq;
                c = array[x];
            }
        }// the end of traversing the suffix

    } else { // bi > bj
        c = 0;  // candidate
        freq_c = 0;
        for (int x = start_index; x <= end_index; x++) {       // start to traverse the query range to find the candidate
            int k = findLeftBound(array[x - 1], start_index);
            int l = findRightBound(array[x - 1], end_index);
            int freq = l - k + 1;
            if (freq > freq_c) {
                freq_c = freq;
                c = array[x];
            }
        }// the end of traversing the query range
    }
    res = make_pair(c, freq_c);
    return res;
}
void ExactMode_Supsr :: getTheSTable() {  // update the S and S' table
    int* countArray = new int[delta + 1];
    for (int i = 0; i < s; i++) {
        int max = 0;
        int mode = 0;
        int count = 0;
        for (int j = i * t ; j < len; j++) {
            countArray[array[j]]++;
            if (countArray[array[j]] > max) {
                max = countArray[array[j]];
                mode = array[j];
            }
            if ((count + 1) % t == 0) {
                S[i][i + ((count + 1) / t) - 1] = mode;
            }
            count++;
        }
        for (int i = 0; i < delta + 1; i++) {
            countArray[i] = 0;
        }
    }
    delete[] countArray;
}

int ExactMode_Supsr :: findLeftBound(int cur, int startIndex) {
    int left = 0, right = QaSize[cur - 1];
    while (left < right) {
        int mid = left + (right - left) / 2;
        if (Qa[cur - 1][mid] >= startIndex) {
            right = mid;
        } else {
            left = mid + 1;
        }
    }
    return left;
}
int ExactMode_Supsr :: findRightBound(int cur, int endIndex) {
    int left = 0, right = QaSize[cur - 1];
    while (left < right) {
        int mid = left + (right - left) / 2;
        if (Qa[cur - 1][mid] <= endIndex) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }
    return right - 1;
}