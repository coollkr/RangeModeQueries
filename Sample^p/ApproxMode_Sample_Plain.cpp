/*
 * It is the file for constructing the data structure and run the query.
 * The abbreviation for this data structure is sample^p,
 * and it is the data structure described in the following paper.
 *
 * Reference:
 * [1] Mark Greve, Allan Grønlund Jørgensen, Kasper Dalgaard Larsen, and Jakob Truelsen. Cell probe lower bounds and approximations for range mode. In International Colloquium on Automata, Languages, and Programming, pages 605–616. Springer, 2010.
 * [2] Simon Gog, Timo Beller, Alistair Moffat, and Matthias Petri. From theory to practice: Plug and play with succinct data structures. In 13th International Symposium on Experimental Algorithms, (SEA 2014), pages 326–337, 2014.
 */
#define MEASURE_SPACE

#include "ApproxMode_Sample_Plain.h"
#include <iostream>
#include <sdsl/bit_vectors.hpp>
#include <bits/stdc++.h>


long long ApproxMode_Sample_Plain :: get1DSize (size_t byte, long row) { // calculate the space usage of 1d array, return value is the space, para: size unit, the number of unit
    long long space = 0;
    space = byte * row;
    return space;
}
long long ApproxMode_Sample_Plain :: get2DSize (size_t byte, long row, long col) { // calculate the space usage of 2d array, return value is the space, para: size unit, the number of unit
    long long space = 0;
    space = byte * row * col;
    return space;
}

ApproxMode_Sample_Plain :: ApproxMode_Sample_Plain(int* array, int len, double epsilon) { // constructor
    this->array = array;
    this->len = len;
    this->epsilon = epsilon;
}
int ApproxMode_Sample_Plain :: construct() { // construct the data structure

#ifdef MEASURE_SPACE
    long long space = 0;
#endif MEASURE_SPACE



    Qi_len = ceil(1 / epsilon);
    long long size = len * Qi_len;

    Qi = new unsigned int[size];
    for (int i = 0; i < size; i++) {
        Qi[i] = INT_MAX;
    }
    delta = 0;
    for (int i = 0; i < len; i++) {
        delta = max(delta, array[i]);
    }




#ifdef MEASURE_SPACE
    space += get2DSize(sizeof(unsigned int), len, Qi_len); //Qi
#endif MEASURE_SPACE





    // update Qi

    int* dict1 = new int[delta + 1];
    int prevJ;
    int temp;

    for (int k = 0; k < Qi_len; k++) {
        for (int i = 0; i < delta + 1; i++) {
            dict1[i] = 0;
        }
        int curK = k + 1;
        int left = 0, right = 0;
        while (left < len) {
            while (right < len) {
                dict1[array[right]]++;
                if (dict1[array[right]] > curK) {
                    Qi[left * Qi_len + k]  = right - 1;
                    dict1[array[right]]--;
                    break;
                } else if (right == len - 1) { // we reach the end and do not exceed the curK
                    Qi[left * Qi_len + k]  = right;

                }
                right++;
            }

            if (right == len) { // right pointer reach the end
                temp = (k==0)? len: prevJ; //if k == 0, prevJ does not have a valid value
                prevJ = left;

                left++;

                while (left  < temp) {
                    Qi[left * Qi_len + k] = len - 1;
                    left++;
                }
                break;
            }
            else {

                dict1[array[left]]--;
                left++;
                while (dict1[array[right]] == curK) {
                    Qi[left * Qi_len + k]  = right - 1;
                    dict1[array[left]]--;
                    left++;
                }
            }
        }
    }

    delete[] dict1;







    // Large Part
    t = ceil(log(epsilon * len) / log(1+epsilon));
    cout << "t: " << t << endl;


    cout << "Qi done!!!!" << endl;

    Ti = new pair<int, int>*[len];
    Ti_len = t;

    TiSize = new int[len]; // mark how many elements in each row should store i.e. store until we meet second element equals the rightmost index


    for (int i = 0; i < len; i++) {
        Ti[i] = new pair<int, int>[Ti_len];
        TiSize[i] = 0;
    }

    for (int k = 0; k < Ti_len; k++) {
        for (int i = 0; i < len; i++) {
            Ti[i][k].second = 0;
        }
    }


    saveK = new int[Ti_len];
    largeK = new int[Ti_len];



    getSaveK();
    getLargeK();


    bool* markSizeAlready = new bool[len]; // mark how many valid element that we should store in each row of Ti table i.e. mark whether we already record the size of current row. (size = the number of elements should store in each row)

    //update Ti and Lik
    int counter = 0;
    int* dict2 = new int[delta + 1];
    for (int k = 0; k < Ti_len; k++) {
        for (int i = 0; i < delta + 1; i++) {
            dict2[i] = 0;
        }
        counter = 0;

        int curK = saveK[k];
        int left = 0, right = 0;
        int prevLikIndex=  0;
        while (left < len) {
            while (right < len) {
                dict2[array[right]]++;
                if (dict2[array[right]] == largeK[k]) {
                    counter++;
                }
                if (dict2[array[right]] > curK) {
                    Ti[left][k].first = right - 1;
                    if (dict2[array[right]] == largeK[k]) {
                        counter--;
                    }
                    dict2[array[right]]--;
                    break;
                } else if (right == len - 1) {
                    Ti[left][k].first = right;
                    if (!markSizeAlready[left]) {
                        markSizeAlready[left] = true;
                        TiSize[left] = k + 1;
                    }
                }
                right++;
            }
            while (Ti[left][k].first != Ti[prevLikIndex][k].first) {
                Ti[prevLikIndex][k].second = array[left - 1];
                prevLikIndex++;
            }
            if (dict2[array[left]] == largeK[k]) {
                counter--;
            }
            dict2[array[left]]--;
            left++;
            if (right == len) {
                while (left < len) {
                    Ti[left][k].first  = len - 1;
                    if (!markSizeAlready[left]) {
                        markSizeAlready[left] = true;
                        TiSize[left] = k + 1;
                    }
                    if (dict2[array[left]] == largeK[k]) {
                        counter--;
                    }
                    dict2[array[left]]--;
                    left++;
                }
            } else {
                while (counter > 0) {
                    Ti[left][k].first  = Ti[left - 1][k].first;
                    if (dict2[array[left]] == largeK[k]) {
                        counter--;
                    }
                    dict2[array[left]]--;
                    left++;
                }
            }
        }
    }
    delete[] dict2;
    delete[] markSizeAlready;



    cout << "Ti and Lik done!!!!" << endl;


    I_len = 1 + ((len - 1) / t);
    k_front_half_bound = ceil(log(t) / log(1 + epsilon));




    I = new int[I_len];
    for (int i = 0; i < I_len; i++) {
        I[i] = i * t;
    }


    bitSize = new int*[k_front_half_bound];
    for (int k = 0; k < k_front_half_bound; k++) {
        bitSize[k] = new int[I_len];
    }

    presumBitSize = new unsigned int*[k_front_half_bound];
    for (int k = 0; k < k_front_half_bound; k++) {
        presumBitSize[k] = new  unsigned int[I_len + 1];
    }


#ifdef MEASURE_SPACE
    space += get2DSize(sizeof(unsigned int), k_front_half_bound, I_len + 1); // presumBitSize
    space += get1DSize(sizeof(unsigned int*), k_front_half_bound); // presumBitSize pointer
#endif MEASURE_SPACE


    tmpbitArray = new bit_vector**[k_front_half_bound];
    for (int k = 0; k < k_front_half_bound; k++) {
        tmpbitArray[k] = new bit_vector*[I_len];
    }

    bitArray = new bit_vector*[k_front_half_bound];
#ifdef MEASURE_SPACE
    space += get1DSize(sizeof(bit_vector*), k_front_half_bound); // bitArray pointer
#endif MEASURE_SPACE


    Sl = new pair<int, int>*[I_len];
    SlSize = new int[I_len];


    int tmpCount2 = 0;
    for (int i = 0; i < I_len; i++) { // trim Sl
        tmpCount2 += SlSize[i];
        SlSize[i] = TiSize[I[i]];
        Sl[i] = new pair<int, int>[SlSize[i]];
        for (int k = 0; k < SlSize[i]; k++) {
            Sl[i][k] = Ti[I[i]][k];
        }
    }


#ifdef MEASURE_SPACE
    space += get1DSize(sizeof(pair<int, int>), tmpCount2);  //Sl
    space += get1DSize(sizeof(pair<int, int>*), I_len);
    space += get1DSize(sizeof(int), I_len); // SlSize
#endif MEASURE_SPACE


    pair<unsigned int, unsigned int>*** tmpCl = new pair<unsigned int, unsigned int>**[I_len];
    for (int i = 0; i < I_len; i++) {
        tmpCl[i] = new pair<unsigned int, unsigned int>*[k_front_half_bound];
        for(int j = 0; j < k_front_half_bound; j++) {
            tmpCl[i][j] = new pair<unsigned int, unsigned int>[t + 1];
        }
    }
    ClSize = new int*[k_front_half_bound];
    for (int i = 0; i < k_front_half_bound; i++) {
        ClSize[i] = new int[I_len];
    }
    for (int k = 0; k < k_front_half_bound; k++) {
        for (int i = 0; i < I_len; i++) {
            ClSize[k][i] = 0;
        }
    }

    int tmpIndex = 0;
    int* mergeSize = new int[k_front_half_bound];

    for (int k = 0; k < k_front_half_bound; k++) {
        int bit_size = 0;
        mergeSize[k] = 0;
        for (int i = 0; i < I_len; i++) {
            bitSize[k][i] = 0;
            if (i == I_len - 1) {
                bit_size = (len - (i * t));
            } else {
                bit_size = t;
            }
            int l = I[i];
            tmpbitArray[k][i] = new bit_vector(bit_size);
            if (TiSize[l] > k) {
                bitSize[k][i]++;
            }
            for (int a = 1; a < bit_size; a++) {
                if (TiSize[l + a] > k) {
                    bitSize[k][i]++;
                    if (Ti[l + a - 1][k].first == Ti[l + a][k].first) {
                        (*tmpbitArray[k][i])[a] = 0;
                    } else {
                        (*tmpbitArray[k][i])[a] = 1;
                        ClSize[k][i]++;
                        tmpCl[i][k][ClSize[k][i]] = Ti[l + a][k];
                    }
                }
            }
            mergeSize[k] += bitSize[k][i];
        }
    }
    endK = 0;
    for (int k = 0; k < k_front_half_bound; k++) {
        if (mergeSize[k] > 0) {
            endK = k;
        } else {
            break;
        }
    }

    newRank = new rank_support_v5<1>*[endK + 1];
    for (int k = 0; k < k_front_half_bound; k++) {
        tmpIndex = 0;
        if (mergeSize[k] > 0) {
            bitArray[k] = new bit_vector(mergeSize[k]);
        } else {
            break;
        }

        for (int i = 0; i < I_len; i++) { // merge bit for each k
            if (bitSize[k][i] > 0) {
                for (int q = 0; q < bitSize[k][i]; q++) {
                    bitArray[k][0][tmpIndex++] = (*tmpbitArray[k][i])[q];
                }
            } else {
                break;
            }
        }

        if (mergeSize[k] > 0) {  // update rank array
            newRank[k] = new rank_support_v5<1>(bitArray[k]);
#ifdef MEASURE_SPACE
            space += size_in_bytes(newRank[k][0]); // newRank
            space += size_in_bytes(bitArray[k][0]); // bitArray
#endif MEASURE_SPACE

        }
    }



    for (int k = 0; k < k_front_half_bound; k++) {
        for (int i = 0; i <= I_len; i++) {
            presumBitSize[k][i] = 0;
        }
    }
    for (int k = 0; k < k_front_half_bound; k++) {
        presumBitSize[k][0] = 0;
        for (int i = 1; i <= I_len; i++) {
            presumBitSize[k][i] += presumBitSize[k][i - 1] + bitSize[k][i - 1];
        }
    }

    cout << "Bl done!!!!" << endl;




    presumClSize = new unsigned int*[I_len];
    for (int i = 0; i < I_len; i++) {
        presumClSize[i] = new unsigned int[k_front_half_bound + 1];
    }
    for (int i = 0; i < I_len; i++) {
        for (int j = 0; j <= k_front_half_bound; j++) {
            presumClSize[i][j] = 0;
        }
    }
    for (int i = 0; i < I_len; i++) {
        for (int j = 1; j <= k_front_half_bound; j++) {
            presumClSize[i][j] += presumClSize[i][j - 1] + ClSize[j - 1][i];
        }
    }



#ifdef MEASURE_SPACE
    space += get2DSize(sizeof(unsigned int), I_len, k_front_half_bound); // presumClSize
    space += get1DSize(sizeof(unsigned int*), I_len); // pointer
#endif MEASURE_SPACE


    int Clcount = 0;

    Cl = new pair<unsigned int, unsigned int>*[I_len];

    for (int i = 0; i < I_len; i++) {
        Cl[i] = new pair<unsigned int, unsigned int>[presumClSize[i][k_front_half_bound]];
        Clcount += presumClSize[i][k_front_half_bound];
    }

#ifdef MEASURE_SPACE
    space += (sizeof(pair<unsigned int, unsigned int>) * Clcount);  //Cl
    space += get1DSize(sizeof(pair<unsigned int, unsigned int>*), I_len);
#endif MEASURE_SPACE


    // update Cl

    for (int i = 0; i < I_len; i++) {
        int index = 0;
        for(int j = 0; j < k_front_half_bound; j++) {
            if (ClSize[j][i] > 0) {
                for (int a = 1; a <= ClSize[j][i]; a++) {
                    Cl[i][index++] = tmpCl[i][j][a];
                }
            }
        }
    }


    delete[] mergeSize;
    for (int i = 0; i < I_len; i++) {
        for(int j = 0; j < k_front_half_bound; j++) {
            delete[] tmpCl[i][j];
        }
        delete[] tmpCl[i];
    }
    delete[] tmpCl;
    cout << "Cl done!!!!" << endl;



    Dl_start_len = ceil(log(t) / log(1 + epsilon)) + 1;

    Dl_len = t - Dl_start_len;




    Dl = new int*[I_len];
    El = new pair<int, int>*[I_len];


    DlSize = new int[I_len];
    int** tmpDl = new int*[I_len];


#ifdef MEASURE_SPACE
    space += get1DSize(sizeof(int*), I_len);  //Dl pointer
    space += get1DSize(sizeof(pair<int, int>*), I_len);//El pointer
    space += get1DSize(sizeof(int), I_len);  //DlSize
#endif MEASURE_SPACE


    if (ceil(log(t) / log(1 + epsilon)) < t) {

        for (int i = 0; i < I_len; i++) {
            tmpDl[i] = new int[Dl_len];
            DlSize[i] = 0;
        }


        for (int j = 0; j < Dl_len; j++) {
            int k = j + Dl_start_len - 1;
            int left = 0, right = I[left];
            while (left < I_len) {

                int l = I[left];
                if (l >= right) {
                    right = l + 1;
                } else {
                    if (right >= len) {
                        tmpDl[left][j] = INT_MAX;
                        break;
                    } else {
                        tmpDl[left][j] = right;
                        DlSize[left]++;
                    }
                    left++;
                    continue;
                }

                while (right < len) {
                    if (this->Ti[l][k].first != this->Ti[right][k].first) {
                        tmpDl[left][j] = right;
                        DlSize[left]++;
                        left++;
                        break;
                    } else {
                        right++;
                        if (right >= len) {
                            tmpDl[left][j] = INT_MAX;
                            break;
                        }
                    }
                }
            }
        }

        cout << "Dl done!!!!" << endl;

        // update trimmed the Dl and El
        int tmpCount1 = 0;
        for (int i = 0; i < I_len; i++) {
            if (DlSize[i] != 0) {
                Dl[i] = new int[DlSize[i]];
                El[i] = new pair<int, int>[DlSize[i]];
                tmpCount1 += DlSize[i];
            }
        }
        for (int i = 0; i < I_len; i++) {
            for (int j = 0; j < Dl_len; j++) {
                if (j < DlSize[i]) {
                    Dl[i][j] = tmpDl[i][j];
                    El[i][j] = Ti[tmpDl[i][j]][j + Dl_start_len - 1];
                }
            }
        }
#ifdef MEASURE_SPACE
        space += get1DSize(sizeof(int), tmpCount1);  //Dl
        space += get1DSize(sizeof(pair<int, int>), tmpCount1);//El
#endif MEASURE_SPACE



        for (int i = 0; i < I_len; i++) {
            delete[] tmpDl[i];
        }
    }

    cout << endl;




    for (int i = 0; i < Ti_len; i++) {
        delete[] Ti[i];
    }
    delete[] Ti;
    cout << "EL done!!!!" << endl;

    findTi(1, 1);

#ifdef MEASURE_SPACE
    cout << "space in bytes: " << space << endl;
#endif MEASURE_SPACE


    delete[] tmpDl;


    return t;
}

ApproxMode_Sample_Plain :: ~ApproxMode_Sample_Plain() { // destructor
    delete[] saveK;
    delete[] largeK;
    for (int i = 0; i < I_len; i++) {
        delete [] Sl[i];
    }
    delete[] Sl;
    for (int i = 0; i < I_len; i++) {
        if (DlSize[i] > 0) {
            delete[] El[i];
        }
    }
    delete[] El;
    for (int i = 0; i < I_len; i++) {
        if (DlSize[i] > 0) {
            delete[] Dl[i];
        }
    }
    delete[] Dl;


    for (int i = 0; i < I_len; i++) {

        delete[] Cl[i];
    }
    delete[] Cl;

    for (int i = 0; i < k_front_half_bound; i++) {
        delete[] ClSize[i];
    }
    delete[] ClSize;
    for (int k = 0; k < k_front_half_bound; k++) {
        delete[] presumBitSize[k];
    }
    delete[] presumBitSize;
    for (int k = 0; k < k_front_half_bound; k++) {
        delete[] bitSize[k];
    }
    delete[] bitSize;
    delete[] bitArray;
    delete[] newRank;
    for (int k = 0; k < I_len; k++) {
        delete[] presumClSize[k];
    }
    delete[] presumClSize;

    delete[] I;
    for (int k = 0; k < k_front_half_bound; k++) {
        delete[] tmpbitArray[k];
    }
    delete[] tmpbitArray;
    delete[] DlSize;
    delete[] TiSize;
    delete[] SlSize;
    delete[] Qi;
}
int ApproxMode_Sample_Plain :: findQueryResult(int start_index, int end_index) { // return the mode, parameter: left index in the query and right index in the query
    int left = start_index - 1, right = end_index - 1;
    int finalMode = -1;
    int key = successor_search(left, right);
    if (key < Qi_len) { // the result in the small frequency
        if (key == 0) {
            return array[left];
        } else {
            return array[Qi[left * Qi_len + key - 1 ] + 1];
        }
    } else { // the result in the large frequency
        int resK = resultBinarySearch(left, right);
        if (resK == 1) {
            int cur = Qi_len - 1;
            finalMode = Qi[left * Qi_len + cur - 1] + 1;
            finalMode = array[finalMode];
        } else {
            int tmp = findTi(left, resK - 1).second;
            finalMode = tmp;
        }
    }
    return finalMode;
}


void ApproxMode_Sample_Plain :: getSaveK() { // update saveK array
    double prevSave =  (1 + epsilon) * (1 + epsilon);
    saveK[0] = ceil((1 / epsilon) * prevSave) - 1;
    for (int i = 1; i < Ti_len; i++) {
        prevSave = prevSave * (1 + epsilon);
        saveK[i] = ceil((1 / epsilon) * prevSave) - 1;
    }
}
void ApproxMode_Sample_Plain :: getLargeK() { // update saveK array
    double prevLarge = 1 + epsilon;
    largeK[0] = ceil((1 / epsilon) * prevLarge) + 1;
    for (int i = 1; i < Ti_len; i++) {
        prevLarge = prevLarge * (1 + epsilon);
        largeK[i] = ceil((1 / epsilon) * prevLarge) + 1;
    }
}
pair<int, int> ApproxMode_Sample_Plain :: findTi(int i, int k) { // get the value in original Ti table based on i and k
    pair<int, int> res = make_pair(-1, -1);
    int real_l = (i / t);
    int l = (i / t) * t;


    if (k > k_front_half_bound) {
        if (DlSize[real_l] <= k - Dl_start_len) {
            return make_pair(INT_MAX, 0);
        }
        if (Dl[real_l][k - Dl_start_len] <= i) {
            return El[real_l][k - Dl_start_len];
        } else {
            if (k - 1 >= SlSize[real_l]) {
                return make_pair(len - 1, -1);
            }
            return Sl[real_l][k - 1];
        }
    } else {

        int start = presumBitSize[k - 1][real_l];
        int first = start + 1;
        int second = start + i - l + 1;


        int size = 0;
        if (k - 1 > endK) {
            size = 0;
        } else {
            size = newRank[k - 1][0].size();
        }

        int r = 0;


        if (first <= size && second <= size) {
            r = newRank[k - 1][0](start + i - l + 1) - newRank[k - 1][0](start + 1);
        } else if (first <= size && second > size) {
            r = newRank[k - 1][0](size) - newRank[k - 1][0](start + 1);
        } else if (first > size && second > size) {
            return make_pair(len - 1, -1);
        }


        if (r != 0) {
            return Cl[real_l][presumClSize[real_l][k - 1] + r - 1];
        } else {
            if (k - 1 >= SlSize[real_l]) {
                return make_pair(len - 1, -1);
            }
            return Sl[real_l][k - 1];
        }
    }
    return res;
}

int ApproxMode_Sample_Plain :: successor_search(int i, int j) { // binary search in Qi table
    // larger or equal smallest
    int left = 0, right = Qi_len; //here right must be Qi_len, not Qi_len-1.
    while (left < right) {
        int mid = left + (right - left) / 2;
        if (Qi[i * Qi_len + mid] < j) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }
    return left;
}
int ApproxMode_Sample_Plain :: resultBinarySearch(int left, int right) { // binary search in Ti table
    int leftK = 0, rightK = t;
    while (leftK < rightK) {
        int midK = leftK + (rightK - leftK) / 2;
        int TiMidK = findTi(left, midK + 1).first;
        if (TiMidK < right) {
            leftK = midK + 1;
        } else {
            rightK = midK;
        }
    }
    return leftK + 1;
}
