/*
 * It is the file for constructing the data structure and run the query.
 * The abbreviation for this data structure is succ^p,
 * and it is the data structure described in the following paper.
 *
 * Reference:
 * [1] Hicham El-Zein, Meng He, J Ian Munro, Yakov Nekrich, and Bryce Sandlund. On approximate range mode and range selection. In 30th International Symposium on Algorithms and Computation (ISAAC 2019), volume 149, page 57. Schloss Dagstuhl–LeibnizZentrum fuer Informatik, 2019.
 * [2] Simon Gog, Timo Beller, Alistair Moffat, and Matthias Petri. From theory to practice: Plug and play with succinct data structures. In 13th International Symposium on Experimental Algorithms, (SEA 2014), pages 326–337, 2014.
 */

#define MEASURE_SPACE
#include "ApproxMode_Succ_Plain.h"
#include <iostream>
#include <sdsl/bit_vectors.hpp>






ApproxMode_Succ_Plain :: ApproxMode_Succ_Plain(int* array, int len, double epsilon) { // constructor
    this->array = array;
    this->len = len;
    this->epsilon = epsilon;
}

void ApproxMode_Succ_Plain :: construct() { // construct the data structure




    Qk_len = ceil(1 / epsilon);
    int **Qi = new int*[Qk_len];

    for (int i = 0; i < Qk_len; i++) {
        Qi[i] = new int[len];
    }
    for (int i = 0; i < Qk_len; i++) {
        for (int j = 0; j < len; j++) {
            Qi[i][j] = -1;
        }
    }


    delta = 0;
    for (int i = 0; i < len; i++) {
        delta = max(delta, array[i]);
    }



#ifdef MEASURE_SPACE
    long long space = 0;
#endif MEASURE_SPACE











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

    int *bitSize = new int[Qk_len];
    for (int k = 0; k < Qk_len; k++) {
        bitSize[k] = 0;
        for (int j = len - 1; j >= 0; j--) {
            if (Qi[k][j] != -1) {
                bitSize[k] = Qi[k][j] + j + 1;
                break;
            }
        }
    }


    //change Qi table to bit vectors array

    Qk = new bit_vector*[Qk_len];
    Qk_select = new select_support_mcl<0>*[Qk_len];
    Qk_rank_1 = new rank_support_v5<1>*[Qk_len];
    numberOfZeroInQk = new int[Qk_len];

#ifdef MEASURE_SPACE
    space += (Qk_len * sizeof(bit_vector*));  // Qk pointer
    space += (Qk_len * sizeof(select_support_mcl<0>*)); // Qk_select pointer
    space += (Qk_len * sizeof(rank_support_v5<0>*)); // Qk_rank_1 pointer
    space += (Qk_len * sizeof(int)); // numberOfZeroInQk
#endif MEASURE_SPACE

    for (int k = 0; k < Qk_len; k++) {
        Qk[k] = new bit_vector(bitSize[k]);
        int index = 0, pre = 0, numberOfZero = 0;
        for (int i = 0; i < len; i++) {
            if (Qi[k][i] != -1) {
                int count = Qi[k][i] - pre;
                for (int c = 0; c < count; c++) {
                    (*Qk[k])[index++] = 1;
                }
                (*Qk[k])[index++] = 0;
                numberOfZero++;
                pre = Qi[k][i];
            }
        }

        Qk_select[k] = new select_support_mcl<0>(Qk[k]);
        Qk_rank_1[k] = new rank_support_v5<1>(Qk[k]);
        numberOfZeroInQk[k] = numberOfZero;

#ifdef MEASURE_SPACE
        space += size_in_bytes(Qk[k][0]);
        space += size_in_bytes(Qk_select[k][0]);
        space += size_in_bytes(Qk_rank_1[k][0]);
#endif MEASURE_SPACE
    }


    cout << "QK done !!! " << endl;


    for (int i = 0; i < Qk_len; i++) {
        delete[] Qi[i];
    }
    delete[] Qi;
    delete[] bitSize;





    //Large Frequence
    k_upper_bound = (log(epsilon * len) / log(1 + epsilon));


    Kr = new bit_vector**[k_upper_bound];
    for (int i = 0; i < k_upper_bound; i++) {
        Kr[i] = new bit_vector*[2];
    }
    Kr_select = new select_support_mcl<1>**[k_upper_bound];
    for (int i = 0; i < k_upper_bound; i++) {
        Kr_select[i] = new select_support_mcl<1>*[2];
    }
    Kr_rank_0 = new rank_support_v5<0>**[k_upper_bound];
    for (int i = 0; i < k_upper_bound; i++) {
        Kr_rank_0 [i] = new rank_support_v5<0>*[2];
    }
    numberOfOneInKr = new int*[k_upper_bound];
    for (int i = 0; i < k_upper_bound; i++) {
        numberOfOneInKr[i] = new int[2];
    }

    cout << "t: " << k_upper_bound << endl;


#ifdef MEASURE_SPACE
    space += (k_upper_bound * sizeof(bit_vector*));  // Kr pointer
    space += (k_upper_bound * sizeof(select_support_mcl<1>*)); // Kr select pointer
    space += (k_upper_bound * sizeof(rank_support_v5<0>*)); // Kr rank 0 pointer
    space += (k_upper_bound * sizeof(int*));// numberOfOneInKr pointer
    space += (k_upper_bound * 2 * sizeof(int));// numberOfOneInKr
#endif MEASURE_SPACE

    int** siArray = new int*[k_upper_bound];
    int** sjArray = new int*[k_upper_bound];

    ceil_f_2k_minus_1Array = new int[k_upper_bound];
    ceil_f_2kArray = new int[k_upper_bound];



#ifdef MEASURE_SPACE
    space += (sizeof(int) * k_upper_bound); // ceil_f_2k_minus_1Array
    space += (sizeof(int) * k_upper_bound); // ceil_f_2kArray
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








    for (int k = 0; k < k_upper_bound; k++) {
        // convert the ri,rj array into bits

        int ceil_f_2k_minus_1 = ceil_f_2k_minus_1Array[k];
        int ceil_f_2k = ceil_f_2kArray[k];
        int count = 0;

        Kr[k][0] = new bit_vector(len + len / ceil_f_2k_minus_1 + 1);


        int prevRi = 0;
        int index = 0;
        int lastOne = 0;
        for (int i = 0; i <= len / ceil_f_2k_minus_1; i++) {
            int si = i * ceil_f_2k_minus_1;
            if (si >= len) {
                break;
            }


            int ri = siArray[k][i];

            if (ri != -1) {
                int diff = ri - prevRi;
                index += diff;
                lastOne = index;
                for (int q = index; q < diff; q++) {
                    (*Kr[k][0])[q++] = 0;
                }
                (*Kr[k][0])[index++] = 1;
                count++;
            } else {
                break;
            }
            prevRi = ri;
        }
        lastOne++;

        (*Kr[k][0]).resize(lastOne);
        numberOfOneInKr[k][0] = count;



        Kr_select[k][0] = new select_support_mcl<1>(Kr[k][0]);
        Kr_rank_0[k][0] = new rank_support_v5<0>(Kr[k][0]);




#ifdef MEASURE_SPACE
        space += size_in_bytes(Kr[k][0][0]);
        space += size_in_bytes(Kr_select[k][0][0]);
        space += size_in_bytes(Kr_rank_0[k][0][0]);
#endif MEASURE_SPACE

        count = 0;
        lastOne = 0;

        int prevRj = 0;
        index = 0;

        Kr[k][1] = new bit_vector(len + len / ceil_f_2k + 1);

        for (int i = 0; i <= len / ceil_f_2k; i++) {
            int sj = i * ceil_f_2k;
            if (sj >= len) {
                break;
            }


            int rj = sjArray[k][i];


            if (rj != -1) {
                int diff = rj - prevRj;
                index += diff;
                lastOne = index;
                for (int q = index; q < diff; q++) {
                    (*Kr[k][1])[q++] = 0;
                }
                (*Kr[k][1])[index++] = 1;
                count++;
            } else {
                break;
            }
            prevRj = rj;

        }
        lastOne++;

        numberOfOneInKr[k][1] = count;
        (*Kr[k][1]).resize(lastOne);



        Kr_select[k][1] = new select_support_mcl<1>(Kr[k][1]);
        Kr_rank_0[k][1] = new rank_support_v5<0>(Kr[k][1]);




#ifdef MEASURE_SPACE
        space += size_in_bytes(Kr[k][1][0]);
        space += size_in_bytes(Kr_select[k][1][0]);
        space += size_in_bytes(Kr_rank_0[k][1][0]);

#endif MEASURE_SPACE


    }
    cout << "Kr done !!!! " << endl;

    delete[] dictArray;




#ifdef MEASURE_SPACE
    cout << "space in bytes: " << space << endl;
#endif MEASURE_SPACE


    for (int k = 0; k < k_upper_bound; k++) {
        delete[] siArray[k];
        delete[] sjArray[k];
    }
    delete[] siArray;
    delete[] sjArray;
}
int ApproxMode_Succ_Plain :: getIndexValue(int index) { // return the value of array[i]
    return array[index];
}
int ApproxMode_Succ_Plain :: findQueryResult(int start_index, int end_index) {  // return the mode parameter: left index in the query and right index in the query
    int left = start_index - 1, right = end_index - 1;
    int tmpResult = findLowFreq(left, right);
    int finalMode = -1;
    if (tmpResult < Qk_len) { //  the result in low frequency
        if (tmpResult - 1 == 0) {
            return array[left];
        } else {
            int index = getQki(tmpResult - 2, left);
            return array[index + 1];
        }
    } else { // high frequency
        finalMode = findLargeMode(left, right);
    }
    return finalMode;
}
ApproxMode_Succ_Plain :: ~ApproxMode_Succ_Plain() { // destructor

    for (int i = 0; i < k_upper_bound; i++) {
        delete[] Kr[i];
    }
    delete[] Kr;
    for (int i = 0; i < k_upper_bound; i++) {
        delete[] Kr_select[i];
    }
    delete[] Kr_select;
    for (int i = 0; i < k_upper_bound; i++) {
        delete[] Kr_rank_0[i];
    }
    delete[] Kr_rank_0;
    delete[] Qk;
    delete[] Qk_select;
    delete[] Qk_rank_1;
    delete[] numberOfZeroInQk;
    for (int i = 0; i < k_upper_bound; i++) {
        delete[] numberOfOneInKr[i];
    }
    delete[] numberOfOneInKr;
    delete[] ceil_f_2k_minus_1Array;
    delete[] ceil_f_2kArray;


}
int ApproxMode_Succ_Plain :: findLargeMode(int start, int end) {  // binary search in large frequency, parameter: left index in the query and right index in the query
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
        int index = getQki(Qk_len - 2, start);
        return array[index + 1];
    }
}

int ApproxMode_Succ_Plain :: getRi(int curK, int i){ // get the ri based on k and si
    int res = len;
    int tmp = numberOfOneInKr[curK][0];
    if (tmp < i + 1) {
        return res;
    }
    int pos = Kr_select[curK][0][0](i + 1);
    res = Kr_rank_0[curK][0][0](pos);
    return res;
}
int ApproxMode_Succ_Plain :: getRj(int curK, int j){ // get the rj based on k and sj
    int res = len;
    int tmp = numberOfOneInKr[curK][1];
    if (tmp < j + 1) {
        return res;
    }
    int pos = Kr_select[curK][1][0](j + 1);
    res = Kr_rank_0[curK][1][0](pos);
    return res;
}
inline int ApproxMode_Succ_Plain :: findSi(int start, int k) {// return si value based on current left index and k
    // get the si value based on k and left index
    return start / ceil_f_2k_minus_1Array[k];
}

inline int ApproxMode_Succ_Plain :: findSj(int start, int k) { // get the sj value based on k and left index
    return start / ceil_f_2kArray[k];
}

int ApproxMode_Succ_Plain :: findLowFreq(int start, int end ) { // binary search in the Qk
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
int ApproxMode_Succ_Plain :: getQki(int k, int i) { // get the value in the Qk table based on k and left index
    int tmp = numberOfZeroInQk[k];
    if (tmp < i + 1) {
        return -1;
    }
    int pos_i_0 = Qk_select[k][0](i + 1); // i + 1
    int num_1 = Qk_rank_1[k][0](pos_i_0);
    return num_1;
}

