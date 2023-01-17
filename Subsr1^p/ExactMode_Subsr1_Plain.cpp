/*
 * It is the file for constructing the data structure and run the query.
 * The abbreviation for this data structure is subsr^1_p,
 * and it is one of the data structures described in the following paper.
 *
 * Reference: [1] Timothy M. Chan, Stephane Durocher, Kasper Green Larsen, Jason Morrison, and Bryan T. Wilkinson. Linear-space data structures for range mode query in arrays. Theory of Computing Systems, 55(4):719–741, Mar 2013.
 * [2] Simon Gog, Timo Beller, Alistair Moffat, and Matthias Petri. From theory to practice: Plug and play with succinct data structures. In 13th International Symposium on Experimental Algorithms, (SEA 2014), pages 326–337, 2014.
 */

#define MEASURE_SPACE

#include "ExactMode_Subsr1_Plain.h"
#include <iostream>

using namespace std;



ExactMode_Subsr1_Plain :: ExactMode_Subsr1_Plain(int* array, int len, int s) { // constructor
    this->array = array;
    this->len = len;
    this->s = s;

}
ExactMode_Subsr1_Plain :: ~ExactMode_Subsr1_Plain() { // destructor
    if (B0_len != 0 && B1_len != 0) {
        for (int i = 0; i < 2; i++) {
            delete[] M[i];
        }
        delete[] M;
        for (int i = 0; i < 2; i++) {
            delete[] I[i];
        }
        delete[] I;
        for (int i = 0; i < 2; i++) {
            delete[] J[i];
        }
        delete[] J;
        for (int i = 0; i < numOfBlocks; i++) {
            delete[] Ci[i];
        }
        delete[] Ci;
        delete[] B0;
        for (int i = 0; i < blocks; i++) {
            delete realBitArray[i];
            delete select_0_array[i];
            delete select_1_array[i];
            delete bit_rank_0_array[i];
            delete bit_rank_1_array[i];
        }
        delete[] realBitArray;
        delete[] select_0_array;
        delete[] select_1_array;
        delete[] bit_rank_0_array;
        delete[] bit_rank_1_array;
        delete[] QaSize;
        delete[] B1_Prime;

        for (int i = 0; i < delta_B1; i++) {
            delete[] Qa_B1[i];
        }
        delete[] Qa_B1;
        delete[] B1;
    } else if (B0_len != 0) {
        for (int i = 0; i < numOfBlocks; i++) {
            delete[] Ci[i];
        }
        delete[] Ci;
        delete[] B0;
    } else if (B1_len != 0) {
        for (int i = 0; i < blocks; i++) {
            delete realBitArray[i];
            delete select_0_array[i];
            delete select_1_array[i];
            delete bit_rank_0_array[i];
            delete bit_rank_1_array[i];
        }
        delete[] realBitArray;
        delete[] select_0_array;
        delete[] select_1_array;
        delete[] bit_rank_0_array;
        delete[] bit_rank_1_array;
        delete[] QaSize;
        delete[] B1_Prime;

        for (int i = 0; i < delta_B1; i++) {
            delete[] Qa_B1[i];
        }
        delete[] Qa_B1;
        delete[] B1;
    }

}
void ExactMode_Subsr1_Plain :: construct() { // construct the data structure

#ifdef MEASURE_SPACE
    long long space = 0;
#endif MEASURE_SPACE



    int delta = 0;
    for (int i = 0; i < len; i++) {
        delta = max(delta, array[i]); // obtain the number of distinct values
    }



    int* tmpCountArray = new int[delta + 1];  // record the occurence of each value
    int* tmpExchangeArray_B0 = new int[delta + 1]; // record the value in B0 array -> new ranked value
    int* tmpExchangeArray_B1 = new int[delta + 1]; // record the value in B1 array -> new ranked value
    for (int i = 0; i < delta + 1; i++) {
        tmpCountArray[i] = 0;
        tmpExchangeArray_B0[i] = 0;
        tmpExchangeArray_B1[i] = 0;
    }

    for (int i = 0; i < len; i++) {
        tmpCountArray[array[i]]++;
    }


    B0_len = 0;  // large freq
    B1_len = 0;  // small freq
    for (int i = 0; i < len; i++) {
        if (tmpCountArray[array[i]] > s) {
            B0_len++;
        } else {
            B1_len++;
        }
    }

    t = (B1_len + s - 1) / (s);  // calculate t value

    delta_B1 = 0, delta_B0 = 0;
    for (int i = 1; i < delta + 1; i++) {
        if (tmpCountArray[i] <= s) {
            delta_B1++;
        } else {
            delta_B0++;
        }
    }


    if (B0_len == 0 || B1_len == 0) {
        I = nullptr;
        J = nullptr;
        M = nullptr;
    } else {
#ifdef MEASURE_SPACE
        space += get1DSize(sizeof(int*), 2); // I pointer
        space += get1DSize(sizeof(int*), 2); // J pointer
        space += get2DSize(sizeof(int), len, 2); // I
        space += get2DSize(sizeof(int), len, 2); // J
        space += get1DSize(sizeof(int*), 2);  // M pointer
        space += get1DSize(sizeof(int), delta_B0);  // M[0]
        space += get1DSize(sizeof(int), delta_B1); // M[1]
#endif MEASURE_SPACE
        I = new int*[2];
        I[0] = new int[len];
        I[1] = new int[len];
        J = new int*[2];
        J[0] = new int[len];
        J[1] = new int[len];
        M = new int*[2];  // store the new-old element change pair
        M[0] = new int[delta_B0];
        M[1] = new int[delta_B1];
    }



#ifdef MEASURE_SPACE

    space += get1DSize(sizeof(int), B1_len);  // B1
    space += get1DSize(sizeof(int), B0_len);  // B0
#endif MEASURE_SPACE


    int count1_B1 = 1;
    int count1_B0 = 1;




    if (B0_len != 0) {
        B0 = new int[B0_len];
    } else {
        B0 = nullptr;
    }
    if (B1_len != 0) {
        B1 = new int[B1_len];
        B1_Prime = new int[B1_len];
        QaSize = new int[delta_B1];
    } else {
        B1 = nullptr;
        B1_Prime = nullptr;
        QaSize = nullptr;
    }

#ifdef MEASURE_SPACE
    space += get1DSize(sizeof(int), B1_len); // A_Prime_B1
    space += get1DSize(sizeof(int), delta_B1); // QaSize
    int count = 0;
#endif MEASURE_SPACE



    if (B1_len != 0) {
        Qa_B1 = new int*[delta_B1]; // initialize 2-D dynamic array Qa
    } else {
        Qa_B1 = nullptr;
    }
    if (B0_len != 0 && B1_len != 0) {
        for (int i = 1; i < delta + 1; i++) {
            if (tmpCountArray[i] <= s) {
                tmpExchangeArray_B1[i] = count1_B1;
                count1_B1++;
                QaSize[tmpExchangeArray_B1[i] - 1] = 0;
                M[1][tmpExchangeArray_B1[i] - 1] = i;
                if (tmpExchangeArray_B1[i] >= 1) {
                    Qa_B1[tmpExchangeArray_B1[i] - 1] = new int[tmpCountArray[i]];
#ifdef MEASURE_SPACE
                    count += tmpCountArray[i];
#endif MEASURE_SPACE
                }
            } else {
                tmpExchangeArray_B0[i] = count1_B0;
                count1_B0++;
                M[0][tmpExchangeArray_B0[i] - 1] = i;
            }
        }
    } else {
        for (int i = 1; i < delta + 1; i++) {
            if (tmpCountArray[i] <= s) {
                if (B1_len == 0) {
                    continue;
                }
                tmpExchangeArray_B1[i] = count1_B1;
                count1_B1++;
                QaSize[tmpExchangeArray_B1[i] - 1] = 0;
                if (tmpExchangeArray_B1[i] >= 1) {
                    Qa_B1[tmpExchangeArray_B1[i] - 1] = new int[tmpCountArray[i]];
#ifdef MEASURE_SPACE
                    count += tmpCountArray[i];
#endif MEASURE_SPACE
                }
            } else {
                if (B0_len == 0) {
                    continue;
                }
                tmpExchangeArray_B0[i] = count1_B0;
                count1_B0++;
            }
        }
    }


    int B0_index = 0, B1_index = 0;
    int B0_index2 = -1, B1_index2 = -1;
    // obtain B, I, J array
    if (B0_len != 0 && B1_len != 0) {
        for (int i = 0; i < len; i++) {
            if (tmpCountArray[array[i]] > s) {
                B0[B0_index] = tmpExchangeArray_B0[array[i]];
                I[0][i] = B0_index;
                I[1][i] = B1_index;
                B0_index++;
                B0_index2++;
                J[0][i] = B0_index2;
                J[1][i] = B1_index2;
            } else {
                B1[B1_index] = tmpExchangeArray_B1[array[i]];
                I[0][i] = B0_index;
                I[1][i] = B1_index;
                B1_index++;
                J[0][i] = B0_index2;
                B1_index2++;
                J[1][i] = B1_index2;
            }
        }
    } else {
        for (int i = 0; i < len; i++) {
            if (tmpCountArray[array[i]] > s) {
                B0[B0_index] = tmpExchangeArray_B0[array[i]];
                B0_index++;
            } else {
                B1[B1_index] = tmpExchangeArray_B1[array[i]];
                B1_index++;
            }
        }
    }


    for (int i = 0; i < B1_len; i++) {  // update Qa_B1
        int cur = B1[i];
        Qa_B1[cur - 1][QaSize[cur - 1]++] = i + 1;
        B1_Prime[i] = QaSize[cur - 1] - 1;
    }




#ifdef MEASURE_SPACE
    space += get1DSize(sizeof(int), count);  // Qa_B1
    space += get1DSize(sizeof(int*), delta_B1); // Qa_B1 pointer
#endif MEASURE_SPACE

    delete[] tmpCountArray;

    realBitArray = nullptr;
    select_0_array = nullptr;
    select_1_array = nullptr;
    bit_rank_0_array = nullptr;
    bit_rank_1_array = nullptr;

    if (B1_len != 0) {  //

        blocks = (B1_len + t - 1) / t;

        realBitArray = new bit_vector*[blocks];
        select_0_array = new select_support_mcl<0>*[blocks];
        select_1_array = new select_support_mcl<1>*[blocks];
        bit_rank_0_array = new rank_support_v5<0>*[blocks];
        bit_rank_1_array = new rank_support_v5<1>*[blocks];


        int* countArray = new int[delta_B1 + 1];


        for (int i = 0; i < blocks; i++) {

            int index = 0;
            realBitArray[i] =  new bit_vector(2 * s + 1, 0);

            int countBlock = 0;

            int preZero = 0;

            for (int i = 0; i < delta_B1 + 1; i++) {
                countArray[i] = 0;
            }
            int max = 0;

            for (int j = i * t ; j < B1_len; j++) {
                countArray[B1[j]]++;
                if (countArray[B1[j]] > max) {
                    max = countArray[B1[j]];
                }
                if ((countBlock + 1) % t == 0) {
                    int numZero = max - preZero;
                    index += numZero;
                    preZero += numZero;
                    realBitArray[i][0][index++] = 1;
                }
                countBlock++;
            }


            realBitArray[i]->bit_resize(index);


            select_0_array[i] = new select_support_mcl<0>(realBitArray[i]);
            select_1_array[i] = new select_support_mcl<1>(realBitArray[i]);
            bit_rank_0_array[i] = new rank_support_v5<0>(realBitArray[i]);
            bit_rank_1_array[i] = new rank_support_v5<1>(realBitArray[i]);


#ifdef MEASURE_SPACE
            space += size_in_bytes(realBitArray[i][0]);
            space += size_in_bytes(select_0_array[i][0]);
            space += size_in_bytes(select_1_array[i][0]);
            space += size_in_bytes(bit_rank_0_array[i][0]);
            space += size_in_bytes(bit_rank_1_array[i][0]);
#endif MEASURE_SPACE
        }  // the end of update bit
        delete[] countArray;



#ifdef MEASURE_SPACE
        space += (sizeof(bit_vector*) * blocks);
        space += (sizeof(select_support_mcl<0>*) * blocks);
        space += (sizeof(select_support_mcl<1>*) * blocks);
        space += (sizeof(rank_support_v5<0>*) * blocks);
        space += (sizeof(rank_support_v5<1>*) * blocks);
#endif MEASURE_SPACE
    } // end of if (B1_len != 0)





    numOfBlocks = 0;
    if (delta_B0 != 0) {
        numOfBlocks = B0_len / delta_B0;
    }


    if (B0_len == 0) {
        Ci = nullptr;
    } else {
        Ci = new int*[numOfBlocks];

        for (int i = 0; i < numOfBlocks; i++) {
            Ci[i] = new int[delta_B0 + 1];
        }
        update_Ci_Table(numOfBlocks); // update Ci table
    }




    cout << "delta_B0: " << delta_B0 << " delta_B1: " << delta_B1 << endl;
    cout << "B0_len: " << B0_len << " B1_len: " << B1_len << endl;
    cout << endl;




#ifdef MEASURE_SPACE
    space += get2DSize(sizeof(int), numOfBlocks, delta_B0 + 1); // Ci space
    space += get1DSize(sizeof(int*), numOfBlocks); //Ci pointer
    cout << "space in bytes: " << space << endl;
#endif MEASURE_SPACE



    delete[] tmpExchangeArray_B0;
    delete[] tmpExchangeArray_B1;
}
pair<int, int> ExactMode_Subsr1_Plain :: findMode(int start_index, int end_index) { // find query result, return value: pair<mode, frequency>, parameter: left index of query and right index of query
    pair<int, int> res;
    int freq_b0 = 0, freq_b1 = 0;
    int mode_b0 = 0, mode_b1 = 0;
    if (B0_len == 0) {
        pair<int, int> resB1 = bitMethod(start_index, end_index, s , t);
        freq_b1 = resB1.second;
        mode_b1 = resB1.first;
    } else if (B1_len == 0) {
        pair<int, int> resB0 = Third_Method( start_index, end_index);
        freq_b0 = resB0.second;
        mode_b0 = resB0.first;
    } else {
        pair<int, int> resB1 = bitMethod(I[1][start_index - 1] + 1, J[1][end_index - 1] + 1, s , t);
        freq_b1 = resB1.second;
        mode_b1 = resB1.first;
        pair<int, int> resB0 = Third_Method( I[0][start_index - 1] + 1, J[0][end_index - 1] + 1);
        freq_b0 = resB0.second;
        mode_b0 = resB0.first;
    }

    int final_freq = 0;
    int final_mode = 0;
    if (freq_b0 >= freq_b1) {
        final_freq = freq_b0;
        final_mode = mode_b0;
    } else {
        final_freq = freq_b1;
        final_mode = mode_b1;
    }
    res.first = final_mode;
    res.second = final_freq;
    return res;
}
pair<int, int> ExactMode_Subsr1_Plain :: Third_Method(int start_index, int end_index) {  // third method part, return value <mode, frequency>, parameter: left index of query and right index of query
    pair<int, int> res;
    int *freq = new int[delta_B0 + 1];
    for (int i = 0; i <= delta_B0; i++) {
        freq[i] = 0;
    }

    int bi = ((start_index-1) + delta_B0 - 1) / delta_B0, bj = (end_index/delta_B0) - 1;

    if (bi <= bj) {  // there are some blocks in the span
        int prefix_start = start_index;
        int prefix_end = min(bi * delta_B0, end_index);
        int suffix_start = max((bj + 1) * delta_B0 + 1, start_index);
        int suffix_end = end_index;
        for (int i = 1; i <= delta_B0; i++) {
            if (bi == 0) {
                freq[i] = Ci[bj][i];
            } else {
                freq[i] = Ci[bj][i] - Ci[bi - 1][i];
            }
        }
        for (int i = prefix_start; i <= prefix_end; i++) {
            freq[B0[i - 1]]++;
        }
        for (int i = suffix_start; i <= suffix_end; i++) {
            freq[B0[i - 1]]++;
        }
    } else { // otherwise, start_index and end_index in the same block
        for (int i = start_index; i <= end_index; i++) {
            freq[B0[i - 1]]++;
        }
    }

    int c = 0, freq_c = 0;
    for (int i = 1; i <= delta_B0; i++) {
        if (freq[i] > freq_c) {
            freq_c = freq[i];
            c = i;
        }
    }


    if (B0_len != 0 && B1_len != 0) {
        c = M[0][c - 1];
    } else {
        c = c;
    }


    res = make_pair(c, freq_c);

    delete[] freq;
    return res;
}
pair<int, int> ExactMode_Subsr1_Plain :: bitMethod(int start_index, int end_index, int s, int t) { // bit method part, return value <mode, frequency>, parameter: left index of query and right index of query

    pair<int, int> res;
    int bi = ((start_index-1) + t - 1) / t, bj = (end_index/t) - 1;
    int span_start = bi * t + 1;
    int span_end = (bj + 1) * t;
    int prefix_start = start_index;
    int prefix_end = min(bi * t, end_index);
    int suffix_start = max((bj + 1) * t + 1, start_index);
    int suffix_end = end_index;

    int c = 0, freq_c = 0;
    if (bi > bj) {
        c = 0;  // candidate
        freq_c = 0;
        for (int x = start_index; x <= end_index; x++) {       // start to traverse the query range to find the candidate
            if (B1_Prime[x-1]-1 >= 0 && Qa_B1[B1[x-1] - 1][B1_Prime[x-1] - 1] >= start_index) {
                continue;
            } else {
                if (B1_Prime[x-1] + freq_c - 1 < QaSize[B1[x-1] - 1] && Qa_B1[B1[x-1] - 1][B1_Prime[x-1] + freq_c - 1] <= end_index) {
                    if (B0_len != 0 && B1_len != 0) {
                        c = M[1][B1[x-1] - 1];
                    } else {
                        c = B1[x-1];
                    }
                    freq_c = getCandidateFreq(B1_len, freq_c, x, start_index ,end_index);
                }
            }
        }// the end of traversing the query range

    } else {

        int pos_bj = select_1_array[bi][0](bj - bi + 1);
        int span_mode = 0;
        int span_freq = bit_rank_0_array[bi][0](pos_bj);
        int pos_last = select_0_array[bi][0](span_freq);
        int b_last = bit_rank_1_array[bi][0](pos_last) + bi;

        int b_last_start = b_last * t + 1, b_last_end = b_last_start + t - 1;

        for (int i = b_last_start; i <= b_last_end; i++) {
            int tmp = B1[i - 1];
            if (B1_Prime[i - 1] - span_freq + 1 >= 0) {
                int value = Qa_B1[tmp - 1][B1_Prime[i - 1] - span_freq + 1];
                if (value >= span_start) {
                    if (B0_len != 0 && B1_len != 0) {
                        span_mode = M[1][B1[i-1] - 1];
                    } else {
                        span_mode = B1[i-1];
                    }
                    break;
                }
            }
        }

        c = span_mode;  // candidate
        freq_c = span_freq;
        for (int x = prefix_start; x <= prefix_end; x++) {       // start to traverse the prefix to find the candidate
            if (B1_Prime[x-1]-1 >= 0 && Qa_B1[B1[x-1] - 1][B1_Prime[x-1] - 1] >= start_index) {
                continue;
            } else {
                if (B1_Prime[x-1] + freq_c - 1 < QaSize[B1[x-1] - 1] && Qa_B1[B1[x-1] - 1][B1_Prime[x-1] + freq_c - 1] <= end_index) {
                    if (B0_len != 0 && B1_len != 0) {
                        c = M[1][B1[x-1] - 1];
                    } else {
                        c = B1[x-1];
                    }
                    freq_c = getCandidateFreq(B1_len, freq_c, x, start_index ,end_index);
                }
            }
        }// the end of traversing the prefix
        for (int x = suffix_end; x >= suffix_start; x--) {       // start to traverse the suffix to find the candidate
            if (B1_Prime[x-1] + 1 < QaSize[B1[x-1] - 1] &&  Qa_B1[B1[x-1] - 1][B1_Prime[x-1] + 1] <= end_index) {
                continue;
            } else {
                if (B1_Prime[x-1] - freq_c + 1 >= 0 && Qa_B1[B1[x-1] - 1][B1_Prime[x-1] - freq_c + 1] >= start_index) {
                    if (B0_len != 0 && B1_len != 0) {
                        c = M[1][B1[x-1] - 1];
                    } else {
                        c = B1[x-1];
                    }
                    freq_c = getSuffixCandidateFreq(B1_len, freq_c, x, start_index ,end_index);
                }
            }
        }// the end of traversing the suffix
    }
    res = make_pair(c, freq_c);
    return res;
}
int ExactMode_Subsr1_Plain :: getCandidateFreq(int len, int freq_c, int x, int i, int j) {  // get the frequency of current candidate, return the frequency of current value -> x in the range from i to j. i: left index, j: right index.
    int currentKey = 0;
    for (int q = B1_Prime[x-1] + freq_c - 1; q  < QaSize[B1[x-1] - 1]; q++) {
        if (Qa_B1[B1[x-1] - 1][q] > j) {
            break;
        }
        currentKey = q;
    }
    int freq = currentKey - B1_Prime[x-1] + 1;
    return freq;
}
int ExactMode_Subsr1_Plain :: getSuffixCandidateFreq(int len, int freq_c, int x, int i, int j) {// get the frequency of current candidate in the suffix , return the frequency of current value -> x in the range from i to j. i: left index, j: right index.
    int currentKey = 0;
    for (int q = B1_Prime[x-1] - freq_c + 1; q  >= 0; q--) {
        if (Qa_B1[B1[x-1] - 1][q]  < i) {
            break;
        }
        currentKey = q;
    }
    int freq = B1_Prime[x-1] - currentKey + 1;
    return freq;
}
void ExactMode_Subsr1_Plain :: update_Ci_Table(int numOfBlocks) {  // update Ci table
    int* freqArray = new int[delta_B0 + 1];
    for (int i = 0; i < delta_B0 + 1; i++) {
        freqArray[i] = 0;
    }
    for (int i = 0; i <  numOfBlocks * delta_B0; i++) {
        freqArray[B0[i]]++;
        if (i % delta_B0 == delta_B0 - 1) {
            for (int j = 1; j < delta_B0 + 1; j++) {
                Ci[i / delta_B0][j] = freqArray[j];
            }
        }
    }
    delete[] freqArray;
}
long long ExactMode_Subsr1_Plain :: get1DSize (size_t byte, int row) { // calculate the space usage of 1d array, return value is the space, para: size unit, the number of unit
    long long space = 0;
    space = byte * row;
    return space;
}
long long ExactMode_Subsr1_Plain :: get2DSize (size_t byte, int row, int col) {// calculate the space usage of 2d array, return value is the space, para: size unit, the number of unit in row and col
    long long space = 0;
    space = byte * row * col;
    return space;
}
