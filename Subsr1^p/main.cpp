/*
 * It is the main.cpp file to run the subsr^1_p data structure
 */

#include <iostream>
#include <cmath>
#include <sys/resource.h>
#include "ExactMode_Subsr1_Plain.h"

using namespace std;

void generate_query(int *tiny_query, int *med_query, int *huge_query, int query);
void read_file(int array[], string file, int len);
void queryExperiment(ExactMode_Subsr1_Plain *exactMode_CDLMW);
double diffUserTime(struct rusage *start, struct rusage *end);
double diffSystemTime(struct rusage *start, struct rusage *end);

int main() {
    int len;   // the length of input array

    //   len = 8571089;
    len = 10000000;
    //   len = 6715122;
    //  len = 7213446;
    int w = 64;
    int s = ceil(sqrt(len * w));      // fixed value : the number of blocks

 
    // cout << s << endl;
    s = 1 * s;

    cout << " s: " << s << endl;
    int *array = new int[len];

    string file_in = "input.txt";


    read_file(array, file_in, len);



    ExactMode_Subsr1_Plain *exactMode_Bit_CDLMW = new ExactMode_Subsr1_Plain(array, len, s);


    struct rusage start, end;

    getrusage(RUSAGE_SELF, &start);
    exactMode_Bit_CDLMW->construct();
    getrusage(RUSAGE_SELF, &end);

    cout << "Construction Time : " << ((double)(diffUserTime(&start, &end) + diffSystemTime(&start, &end)) / 60 ) <<" min " << endl;

    cout << "userTime:  " << diffUserTime(&start, &end) / 60 << endl;
    cout << "sysTime:  " << diffSystemTime(&start, &end) / 60 << endl;

    queryExperiment(exactMode_Bit_CDLMW);

    delete[] array;
    delete exactMode_Bit_CDLMW;

    return 0;
}
void queryExperiment(ExactMode_Subsr1_Plain *exactMode_Bit_CDLMW) {
    int start_index = 0;
    int end_index = 0;
    int *tiny_query = new int[2000000];
    int *med_query = new int[2000000];
    int *huge_query = new int[2000000];
    int *result = new int[3000000];
    generate_query(tiny_query, med_query, huge_query, 2000000);
    int mode = -1, freq = -1;

    struct rusage start, end;

    cout << "Tiny Query is:" <<endl;
    getrusage(RUSAGE_SELF, &start);
    for (int i = 0; i < 2000000; i+=2) {
        start_index = tiny_query[i];
        end_index = tiny_query[i + 1];
        pair<int, int> res = exactMode_Bit_CDLMW->findMode(start_index, end_index);
        mode = res.first;
        freq = res.second;
        result[(i / 2)] = freq;
    }
    getrusage(RUSAGE_SELF, &end);
    cout << " Tiny Query Total Time : " << (double)(diffUserTime(&start, &end) + diffSystemTime(&start, &end)) <<" s " << endl;

    cout << "Med Query is:" <<endl;
    getrusage(RUSAGE_SELF, &start);
    for (int i = 2000000; i < 4000000; i+=2) {
        start_index = med_query[i - 2000000];
        end_index = med_query[i + 1 - 2000000];
        pair<int, int> res = exactMode_Bit_CDLMW->findMode(start_index, end_index);
        mode = res.first;
        freq = res.second;
        result[(i / 2)] = freq;
    }
    getrusage(RUSAGE_SELF, &end);
    cout << " Med Query Total Time : " << (double)(diffUserTime(&start, &end) + diffSystemTime(&start, &end)) <<" s " << endl;

    cout << "Huge Query is:" <<endl;
    getrusage(RUSAGE_SELF, &start);
    for (int i = 4000000; i < 6000000; i+=2) {
        start_index = huge_query[i - 4000000];
        end_index = huge_query[i + 1 - 4000000];
        pair<int, int> res = exactMode_Bit_CDLMW->findMode(start_index, end_index);
        mode = res.first;
        freq = res.second;
        result[(i / 2)] = freq;
    }
    getrusage(RUSAGE_SELF, &end);
    cout << " Huge Query Total Time : " << (double)(diffUserTime(&start, &end) + diffSystemTime(&start, &end)) <<" s " << endl;



    delete[] tiny_query;
    delete[] med_query;
    delete[] huge_query;

    ofstream integrated_result("result.txt");

    for (int i = 0; i < 3000000; i++) {
        integrated_result << result[i] << " ";
        if ((i + 1) % 10 == 0) {
            integrated_result << endl;
        }
    }
    if (integrated_result) {
        cout << endl;
        cout << "create integrated result file successfully!" << endl;
    }
    integrated_result.close();
    integrated_result.clear();
    delete[] result;
}

void generate_query(int *tiny_query, int *med_query, int *huge_query, int query){
    int len = query;

    string tiny_file = "tinyExact.txt";
    string med_file = "medExact.txt";
    string huge_file = "hugeExact.txt";

    ifstream tiny_in(tiny_file);
    ifstream med_in(med_file);
    ifstream huge_in(huge_file);
    if (!tiny_in.is_open()) {
        cout << " cannot open the tiny file" << endl;
    }
    for (int i = 0; i < len; i++) {
        tiny_in >> tiny_query[i];
    }
    if (!med_in.is_open()) {
        cout << " cannot open the med file" << endl;
    }
    for (int i = 0; i < len; i++) {
        med_in >> med_query[i];
    }
    if (!huge_in.is_open()) {
        cout << " cannot open the huge file" << endl;
    }
    for (int i = 0; i < len; i++) {
        huge_in >> huge_query[i];
    }
    tiny_in.close();
    tiny_in.clear();
    med_in.close();
    med_in.clear();
    huge_in.close();
    huge_in.clear();
}
void read_file(int array[], string file, int len) {
    ifstream file_in(file);
    if (!file_in.is_open()) {
        cout << " cannot open the file" << endl;
    }
    for (int i = 0; i < len; i++) {
        file_in >> array[i];
    }
    file_in.close();
    file_in.clear();
}
double diffUserTime(struct rusage *start, struct rusage *end)
{
    return (end->ru_utime.tv_sec - start->ru_utime.tv_sec) +
           1e-6*(end->ru_utime.tv_usec - start->ru_utime.tv_usec);
}

double diffSystemTime(struct rusage *start, struct rusage *end)
{
    return (end->ru_stime.tv_sec - start->ru_stime.tv_sec) +
           1e-6*(end->ru_stime.tv_usec - start->ru_stime.tv_usec);
}





