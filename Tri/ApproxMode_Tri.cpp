/*
 * It is the file for constructing the data structure and run the query.
 * The abbreviation for this data structure is tri,
 * and it is the data structure described in the following paper.
 *
 * Reference:
 * [1] Hicham El-Zein, Meng He, J Ian Munro, Yakov Nekrich, and Bryce Sandlund. On approximate range mode and range selection. In 30th International Symposium on Algorithms and Computation (ISAAC 2019), volume 149, page 57. Schloss Dagstuhl–LeibnizZentrum fuer Informatik, 2019.
 */
#define MEASURE_SPACE

#include "ApproxMode_Tri.h"
#include <iostream>
#include <bits/stdc++.h>


ApproxMode_Tri :: ApproxMode_Tri(int* array, int len, double epsilon) { // constructor
    this->array = array;
    this->len = len;
    this->epsilon = epsilon;
}

void ApproxMode_Tri :: construct() { // construct the data structure



    Qk_len = ceil(1 / epsilon);
    Qi = new int*[Qk_len];

    for (int i = 0; i < Qk_len; i++) {
        Qi[i] = new int[len];
    }
    for (int i = 0; i < Qk_len; i++) {
        for (int j = 0; j < len; j++) {
            Qi[i][j] = -1;
        }
    }
#ifdef MEASURE_SPACE
    long long space = 0;
    space += (sizeof(int) * Qk_len * len); // Qk
    space += (sizeof(int*) * Qk_len); // Qk pointer
#endif MEASURE_SPACE

    delta = 0;
    for (int i = 0; i < len; i++) {
        delta = max(delta, array[i]);
    }




    // update Qi table firstly


    int* dict1 = new int[delta + 1];
    int prevJ;
    int temp;

    for (int k = 0; k < Qk_len; k++) {
        for (int i = 0; i < delta + 1; i++) {
            dict1[i] = 0;
        }
        int curK = k + 1;
        int left = 0, right = 0;
        while (left < len) {
            while (right < len) {
                dict1[array[right]]++;
                if (dict1[array[right]] > curK) {
                    Qi[k][left]  = right - 1;
                    dict1[array[right]]--;
                    break;
                } else if (right == len - 1) { // we reach the end and do not exceed the curK
                    Qi[k][left] = right;

                }
                right++;
            }

            if (right == len) { // right pointer reach the end
                temp = (k==0)? len: prevJ; //if k == 0, prevJ does not have a valid value
                prevJ = left;

                left++;

                while (left  < temp) {
                    Qi[k][left]= len - 1;
                    left++;
                }
                break;
            }
            else {

                dict1[array[left]]--;
                left++;
                while (dict1[array[right]] == curK) {
                    Qi[k][left] = right - 1;
                    dict1[array[left]]--;
                    left++;
                }
            }
        }
    }

    delete[] dict1;






    cout << "QK done !!! " << endl;







    //Large Frequence
    k_upper_bound = (log(epsilon * len) / log(1 + epsilon));






    cout << "t: " << k_upper_bound << endl;




    int** siArray = new int*[k_upper_bound];
    int** sjArray = new int*[k_upper_bound];

    ceil_f_2k_minus_1Array = new int[k_upper_bound];
    ceil_f_2kArray = new int[k_upper_bound];


#ifdef MEASURE_SPACE
    space += (sizeof(int) * k_upper_bound);
    space += (sizeof(int) * k_upper_bound);
#endif MEASURE_SPACE

    int* dictArray = new int[delta + 1];

    double frontPart = ((sqrt(1 + epsilon) - 1) / epsilon);
    double sqrtPart = sqrt(1 + epsilon);
    double prevPart1 = (pow(sqrtPart, 2 * 1 - 1)), prevPart2 = (pow(sqrtPart , 2 * 1));
    double prev2k1 = (pow(sqrtPart, 2 * 1) / epsilon), prev2k2 = (pow(sqrtPart, 2 * 1 + 1) / epsilon);



    for (int k = 0; k < k_upper_bound; k++) { // get the ri, rj value

        for (int i = 0; i < delta + 1; i++) {
            dictArray[i] = 0;
        }


        int ceil_f_2k_minus_1 = 0;
        int ceil_f_2k = 0;
        double freq_2k = 0;

        if (k == 0) {
            ceil_f_2k_minus_1 = ceil(frontPart * prevPart1);
            ceil_f_2k = ceil(frontPart * prevPart2);
            freq_2k = prev2k1;
        } else {
            prevPart1 = prevPart1 * sqrtPart * sqrtPart;
            prevPart2 = prevPart2 * sqrtPart * sqrtPart;
            ceil_f_2k_minus_1 = ceil(frontPart * prevPart1);
            ceil_f_2k = ceil(frontPart * prevPart2);
            freq_2k = prev2k1 * sqrtPart * sqrtPart;
            prev2k1 = freq_2k;
        }

        ceil_f_2k_minus_1Array[k] = ceil_f_2k_minus_1;
        ceil_f_2kArray[k] = ceil_f_2k;

        siArray[k] = new int[len / ceil_f_2k_minus_1 + 1];
        for (int q = 0; q < len / ceil_f_2k_minus_1 + 1; q++) {
            siArray[k][q] = -1;
        }
        int right = 0, i = 0;


        while (i < len / ceil_f_2k_minus_1 + 1) {
            if (right < len && dictArray[array[right]] < freq_2k) {
                if (right != 0) {
                    right++;
                }
                while (right < len) {
                    dictArray[array[right]]++;
                    if (dictArray[array[right]] >= freq_2k) {
                        siArray[k][i] = right;
                        break;
                    }
                    right++;
                }
            }
            if (right == len) {
                break;
            } else {
                int tmp = i * ceil_f_2k_minus_1;
                int next = (i + 1) * ceil_f_2k_minus_1;
                while (tmp < next) {
                    if (tmp >= len){
                        break;
                    }
                    dictArray[array[tmp]]--;
                    tmp++;
                }
                i++;
                if (dictArray[array[right]] >= freq_2k) {
                    siArray[k][i] = right;
                }
            }
        }




        for (int i = 0; i < delta + 1; i++) {
            dictArray[i] = 0;
        }


        sjArray[k] = new int[len / ceil_f_2k + 1];
        for (int q = 0; q < len / ceil_f_2k + 1; q++) {
            sjArray[k][q] = -1;
        }
        right = 0, i = 0;

        if (k == 0) {
            freq_2k = prev2k2;
        } else {
            freq_2k = prev2k2 * sqrtPart * sqrtPart;
        }

        prev2k2 = freq_2k;
        while (i < len / ceil_f_2k + 1) {
            if (right < len && dictArray[array[right]] < freq_2k) {
                if (right != 0) {
                    right++;
                }
                while (right < len) {
                    dictArray[array[right]]++;
                    if (dictArray[array[right]] >= freq_2k) {
                        sjArray[k][i] = right;
                        break;
                    }
                    right++;
                }
            }
            if (right == len) {
                break;
            } else {
                int tmp = i * ceil_f_2k;
                int next = (i + 1) * ceil_f_2k;
                while (tmp < next) {
                    if (tmp >= len){
                        break;
                    }
                    dictArray[array[tmp]]--;
                    tmp++;
                }
                i++;
                if (dictArray[array[right]] >= freq_2k) {
                    sjArray[k][i] = right;
                }
            }
        }




    }


    riSize = new int[k_upper_bound];
    rjSize = new int[k_upper_bound];



#ifdef MEASURE_SPACE
    space += (sizeof(int) * k_upper_bound);
    space += (sizeof(int) * k_upper_bound);
#endif MEASURE_SPACE

    for (int k = 0; k < k_upper_bound; k++) {  // trim the ri,rj array


        int ceil_f_2k_minus_1 = ceil_f_2k_minus_1Array[k];
        int ceil_f_2k = ceil_f_2kArray[k];
        riSize[k] = 0;
        rjSize[k] = 0;
        for (int q = 0; q < len / ceil_f_2k_minus_1 + 1; q++) {
            if (siArray[k][q] != -1) {
                riSize[k]++;
            } else {
                break;
            }
        }
        for (int q = 0; q < len / ceil_f_2k + 1; q++) {
            if (sjArray[k][q] != -1) {
                rjSize[k]++;
            } else {
                break;
            }
        }

    }
    riArray = new int*[k_upper_bound];
    rjArray = new int*[k_upper_bound];
    long riCount = 0, rjCount = 0;
    for (int k = 0; k < k_upper_bound; k++) { // update ri and rj array
        riCount += riSize[k];
        rjCount += rjSize[k];
        if (riSize[k] > 0) {
            riArray[k] = new int[riSize[k]];
            for (int q = 0; q < riSize[k]; q++) {
                riArray[k][q] = siArray[k][q];
            }
        }
        if (rjSize[k] > 0) {
            rjArray[k] = new int[rjSize[k]];
            for (int q = 0; q < rjSize[k]; q++) {
                rjArray[k][q] = sjArray[k][q];
            }
        }

    }




    cout << "Kr done !!!! " << endl;

    delete[] dictArray;

#ifdef MEASURE_SPACE
    space += (sizeof(int) * riCount); // riArray
    space += (sizeof(int) * rjCount); // rjArray
    space += (sizeof(int*) * k_upper_bound); // riArray pointer
    space += (sizeof(int*) * k_upper_bound); // rjArray pointer
    cout << "space in bytes: " << space << endl;
#endif MEASURE_SPACE



    for (int k = 0; k < k_upper_bound; k++) {
        delete[] siArray[k];
        delete[] sjArray[k];
    }
    delete[] siArray;
    delete[] sjArray;
}
int ApproxMode_Tri :: getIndexValue(int index) {  // return the value of array[index]
    return array[index];
}
int ApproxMode_Tri :: findQueryResult(int start_index, int end_index) { // return the mode, parameter: left index in the query and right index in the query
    int left = start_index - 1, right = end_index - 1;
    int tmpResult = findLowFreq(left, right);
    int finalMode = -1;
    if (tmpResult < Qk_len) { //  the result in low frequency
        if (tmpResult - 1 == 0) {
            return array[left];
        } else {
            return array[Qi[tmpResult - 2][left] + 1];
        }
    } else { // high frequency
        finalMode = findLargeMode(left, right);
    }
    return finalMode;
}
ApproxMode_Tri :: ~ApproxMode_Tri() { // destructor
    for (int i = 0; i < Qk_len; i++) {
        delete[] Qi[i];
    }
    delete[] Qi;

    for (int i = 0; i < k_upper_bound; i++) {
        delete[] riArray[i];
        delete[] rjArray[i];
    }

    delete[] riArray;
    delete[] rjArray;
    delete[] riSize;
    delete[] rjSize;
    delete[] ceil_f_2k_minus_1Array;
    delete[] ceil_f_2kArray;


}
int ApproxMode_Tri :: findLargeMode(int start, int end) { // binary search in large frequency  parameter: left index in the query and right index in the query
    int leftK = 0, rightK = k_upper_bound - 1, midK;
    int si, ri, sj, rj;
    while (leftK != rightK) {
        midK = leftK + (rightK - leftK + 1) / 2; // set midK = ceil((leftK + rightK)/2), not floor
        si = findSi(start, midK);
        ri = getRi(midK, si);

        if (end < ri) { // case 1 applies. the value of k that we are looking for is at most midK-1
            rightK = midK - 1;
            continue;
        }

        sj = findSj(start, midK);
        rj = getRj(midK, sj);

        if (end >= rj) { // case 2 applies. the value of k that we are looking for is at least midK
            leftK = midK;
        }
        else { // case 3 applies. return
            return array[ri];
        }
    }

    int sj_leftK, rj_leftK;
    if (leftK == midK) { // avoid recomputing sj and rj
        sj_leftK = sj;
        rj_leftK = rj;
    }
    else {
        sj_leftK = findSj(start, leftK);
        rj_leftK = getRj(leftK, sj_leftK);
    }

    // Note: if leftK > 0 and leftK < k_upper_bound-1, only case 2 can apply for leftK

    // case 2 applies for leftK. In this case, either case 1 applies for leftK+1, for leftK == k_upper_bound-1
    if (end >= rj_leftK)
        return array[rj_leftK];

    //if we have not returned so far, this means leftK == 0, the smallest possible k.
    int si_leftK, ri_leftK;
    if (leftK == midK)  { // avoid recomputing si and ri
        si_leftK = si;
        ri_leftK = ri;
    }
    else {
        si_leftK = findSi(start, leftK);
        ri_leftK = getRi(leftK, si_leftK);
    }

    // case 3 applies for leftK. we already know end < rj_leftK
    if (end >= ri_leftK) {
        return array[ri_leftK];
    }

    // the remaining possiblity is leftK ==0 and case 1 applies to leftK
    // Use the low frequency structure
    if (Qk_len == 1) {
        return array[start];
    } else {
        return array[Qi[Qk_len - 2][start] + 1];
    }
}

inline int ApproxMode_Tri :: getRi(int curK, int i){ // get the ri value based on k and si
    if (i >= riSize[curK]) {
        return len;
    } else {
        return riArray[curK][i];
    }
}

inline int ApproxMode_Tri :: getRj(int curK, int j){ // get the rj value based on k and sj
    if (j >= rjSize[curK]) {
        return len;
    } else {
        return rjArray[curK][j];
    }
}

inline int ApproxMode_Tri :: findSi(int start, int k) {// return si value based on current left index and k
    // get the si value based on k and left index
    return start / ceil_f_2k_minus_1Array[k];
}

inline int ApproxMode_Tri :: findSj(int start, int k) { // get the sj value based on k and left index
    return start / ceil_f_2kArray[k];
}


int ApproxMode_Tri :: findLowFreq(int start, int end ) { // binary search on Qk
    int leftK = 0, rightK = Qk_len;
    while (leftK < rightK) {
        int midK = leftK + (rightK - leftK) / 2;
        int tmp = getQki(midK, start);
        if (tmp >= end  || tmp == -1) {
            rightK = midK;
        } else if (tmp < end) {
            leftK = midK + 1;
        }
    }
    return leftK + 1;
}

inline int ApproxMode_Tri :: getQki(int k, int i) { // get Qk value based on k and i
    return Qi[k][i];
}

