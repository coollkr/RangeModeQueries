/*
 * It is the file for constructing the data structure and run the query.
 * The abbreviation for this data structure is sqrt,
 * and it is the data structure described in the following paper.
 *
 * Reference: Timothy M. Chan, Stephane Durocher, Kasper Green Larsen, Jason Morrison, and Bryan T. Wilkinson. Linear-space data structures for range mode query in arrays. Theory of Computing Systems, 55(4):719–741, Mar 2013.
 *
 */

#define MEASURE_SPACE

#include <iostream>
#include "ExactMode_Sqrt.h"

using namespace std;


ExactMode_Sqrt :: ExactMode_Sqrt(int* array, int len, int s) { // constructor
    this->array = array;
    this->len = len;
    this->s = s;

}
ExactMode_Sqrt :: ~ExactMode_Sqrt() { // destructor
    delete[] QaSize;
    for (int i = 0; i < delta; i++) {
        delete[] Qa[i];
    }
    delete[] Qa;
    for (int i = 0; i < s; i++) {
        delete[] S[i];
    }
    delete[] S;
    for (int i = 0; i < s; i++) {
        delete[] S_freq[i];
    }
    delete[] S_freq;

    delete[] A_Prime;
}
void ExactMode_Sqrt :: construct() { // construct the data structure


#ifdef MEASURE_SPACE
    long long space = 0;
    space += get1DSize(sizeof(int), len);  // input array
#endif MEASURE_SPACE

    delta = 0;
    for (int i = 0; i < len; i++) {  // get the number of distinct value
        delta = max(delta, array[i]);
    }




    A_Prime = new int[len];
    Qa = new int*[delta];            // initialize 2-D dynamic array Qa
    QaSize = new int[delta];


    for (int i = 0; i < delta; i++) {
        QaSize[i] = 0;
    }

    for (int i = 0; i < len; i++) {  // update A_Prime
        A_Prime[i] = QaSize[array[i]-1]++;
    }


#ifdef MEASURE_SPACE
    space += get1DSize(sizeof(int), len); // A_Prime size
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
    S_freq = new int*[s];
    for (int i = 0; i < s; i++) {
        S[i] = new int[s];
    }
    for (int i = 0; i < s; i++) {
        S_freq[i] = new int[s];
    }


    getTheSTable();  // create the table S and S_freq

#ifdef MEASURE_SPACE
    space += get2DSize(sizeof(int), s, s);  // S space
    space += get2DSize(sizeof(int), s, s); // S_freq space
    space += get1DSize(sizeof(int*), s);  // S pointer
    space += get1DSize(sizeof(int*), s);// S_freq pointer
    cout << "space in bytes: " << space << endl;
    return;
#endif MEASURE_SPACE

}
long long ExactMode_Sqrt :: get1DSize (size_t byte, int row) { // calculate the space usage of 1d array, return value is the space, para: size unit, the number of unit
    long long space = 0;
    space = byte * row;
    return space;
}
long long ExactMode_Sqrt :: get2DSize (size_t byte, int row, int col) { // calculate the space usage of 2d array, return value is the space, para: size unit, the number of unit
    long long space = 0;
    space = byte * row * col;
    return space;
}

pair<int, int> ExactMode_Sqrt :: findMode(int start_index, int end_index) {  // find query result, return value: pair<mode, frequency>, para: left index of query and right index of query
    pair<int, int> res;


    int bi = ((start_index-1) + t - 1) / t, bj = (end_index/t) - 1;

    int span_start = bi * t + 1;
    int span_end = (bj + 1) * t;
    int prefix_start = start_index;
    int prefix_end = min(bi * t, end_index);
    int suffix_start = max((bj + 1) * t + 1, start_index);
    int suffix_end = end_index;
    int c = 0;  // candidate
    int freq_c = 0;

    if (bi <= bj) {    // not in one span
        int span_mode = S[bi][bj];
        c = span_mode;  // candidate
        freq_c = S_freq[bi][bj];
        for (int x = prefix_start; x <= prefix_end; x++) {       // start to traverse the prefix to find the candidate
            if (A_Prime[x-1]-1 >= 0 && Qa[array[x-1] - 1][A_Prime[x-1] - 1] >= start_index) {
                continue;
            } else {
                if (A_Prime[x-1] + freq_c - 1 < QaSize[array[x-1] - 1] && Qa[array[x-1] - 1][A_Prime[x-1] + freq_c - 1] <= end_index) {
                    c = array[x-1];
                    freq_c = getCandidateFreq( freq_c, x, start_index ,end_index);
                }
            }
        }// the end of traversing the prefix
        for (int x = suffix_end; x >= suffix_start; x--) {       // start to traverse the suffix to find the candidate
            if (A_Prime[x-1] + 1 < QaSize[array[x-1] - 1] && Qa[array[x-1] - 1][A_Prime[x-1] + 1] <= end_index) {
                continue;
            } else {
                if (A_Prime[x-1] - freq_c + 1 >= 0 && Qa[array[x-1] - 1][A_Prime[x-1] - freq_c + 1] >= start_index) {
                    c = array[x-1];
                    freq_c = getSuffixCandidateFreq( freq_c, x, start_index ,end_index);
                }
            }
        }// the end of traversing the suffix

    } else {
        c = 0;  // candidate
        freq_c = 0;
        for (int x = start_index; x <= end_index; x++) {       // start to traverse the query range to find the candidate
            if (A_Prime[x-1]-1 >= 0 && Qa[array[x-1] - 1][A_Prime[x-1] - 1] >= start_index) {
                continue;
            } else {
                if (A_Prime[x-1] + freq_c - 1 < QaSize[array[x-1] - 1] && Qa[array[x-1] - 1][A_Prime[x-1] + freq_c - 1] <= end_index) {
                    c = array[x-1];
                    freq_c = getCandidateFreq(freq_c, x, start_index ,end_index);
                }
            }
        }// the end of traversing the query range
    }
    res = make_pair(c, freq_c);
    return res;
}
void ExactMode_Sqrt :: getTheSTable() {  // update the S and S' table
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
                S_freq[i][i + ((count + 1) / t) - 1] = max;
            }
            count++;
        }
        for (int i = 0; i < delta + 1; i++) {
            countArray[i] = 0;
        }
    }
    delete[] countArray;
}

int ExactMode_Sqrt :: getCandidateFreq(int freq_c, int x, int i, int j) {  // get the frequency of current candidate, return the frequency of current value -> x in the range from i to j. i: left index, j: right index.
    int currentKey = 0;
    for (int q = A_Prime[x-1] + freq_c - 1; q  < QaSize[array[x-1] - 1]; q++) {
        if (Qa[array[x-1] - 1][q] > j) {
            break;
        }
        currentKey = q;
    }
    int freq = currentKey - A_Prime[x-1] + 1;
    return freq;
}
int ExactMode_Sqrt :: getSuffixCandidateFreq(int freq_c, int x, int i, int j) { // get the frequency of current candidate in the suffix , return the frequency of current value -> x in the range from i to j. i: left index, j: right index.
    int currentKey = 0;
    for (int q = A_Prime[x-1] - freq_c + 1; q  >= 0; q--) {
        if (Qa[array[x-1] - 1][q]  < i) {
            break;
        }
        currentKey = q;
    }
    int freq = A_Prime[x-1] - currentKey + 1;
    return freq;
}
