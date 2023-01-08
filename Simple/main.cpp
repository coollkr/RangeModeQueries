/*
 * It is the main.cpp file to run the simple data structure
 */
#include <iostream>
#include <fstream>
#include <sys/resource.h>
#include "ApproxMode_Simple.h"

using namespace std;

void generate_query(int *tiny_query, int *med_query, int *huge_query, int query);
void read_file(int array[], string file, int len);
void queryExperiment(ApproxMode_Simple *approximateModeBkmt, int delta, int** Qa, int* QaSize);
double diffUserTime(struct rusage *start, struct rusage *end);
double diffSystemTime(struct rusage *start, struct rusage *end);
void read_Qa_file(int** Qa, int* QaSize, string file, int delta);
int leftsideBinarySearch(int start, int row, int* rowQa, int size);
int rightsideBinarySearch(int end, int row, int* rowQa, int size);

int main() {
    int len;   // the length of input array
    cout << "please input the length of the array: ";
    //cin >> len;

    //   len = 8571089;
    len = 6715122;
    // len = 10000000;
    //len = 20;
    double epsilon;
    cout << "please input the alpha: ";
    // cin >> epsilon;
    epsilon = 0.5;
    double alpha = (double) 1 / (1 + epsilon);
    //double alpha = epsilon;
    cout << "alpha: " << alpha << endl;
    int *array = new int[len];
    //   string file_in = "/users/grad/liu1/data_book/book_1000.txt";
    //   string file_in = "/users/grad/liu1/data_ip/ip_857.txt";
    string file_in = "/users/grad/liu1/data_text/text_671.txt";
    //   string file_in = "/users/grad/liu1/data_library/lib_1000.txt";
    // string file_in = "/users/grad/liu1/data_park/park_1000.txt";

    read_file(array, file_in, len);

    int delta = 0;
    for (int i = 0; i < len; i++) {  // get the number of distinct value
        delta = max(delta, array[i]);
    }
    int* QaSize = new int[delta];
    int** Qa = new int*[delta];
    string file_QaSize_in = "/users/grad/liu1/data_text/QaSize.txt";
    read_file(QaSize, file_QaSize_in, delta);

    for (int i = 0; i < delta; i++) {
        Qa[i] = new int[QaSize[i]];
    }

    string file_Qa_in = "/users/grad/liu1/data_text/Qa.txt";
    read_Qa_file(Qa, QaSize, file_Qa_in, delta);

    ApproxMode_Simple *approximateMode_BKMT = new ApproxMode_Simple(array, len, alpha);




    struct rusage start, end;

    getrusage(RUSAGE_SELF, &start);
    approximateMode_BKMT->construct();
    getrusage(RUSAGE_SELF, &end);


    cout << "Construction Time : " << ((double)(diffUserTime(&start, &end) + diffSystemTime(&start, &end)) / 60 ) <<" min " << endl;
    cout << "userTime:  " << diffUserTime(&start, &end) / 60 << endl;
    cout << "sysTime:  " << diffSystemTime(&start, &end) / 60 << endl;

    queryExperiment(approximateMode_BKMT, delta, Qa, QaSize);

    for (int i = 0; i < delta; i++) {
        delete[] Qa[i];
    }
    delete[] Qa;
    delete[] QaSize;
    delete approximateMode_BKMT;
    delete[] array;
    return 0;
}
void queryExperiment(ApproxMode_Simple *approximateMode_BKMT, int delta, int** Qa, int* QaSize) {
    int start_index = 0;
    int end_index = 0;
    int *tiny_query = new int[200000000];
    int *med_query = new int[200000000];
    int *huge_query = new int[200000000];
    int *result = new int[300000000];
    generate_query(tiny_query, med_query, huge_query, 200000000);

    struct rusage start, end;

    cout << "Tiny Query is:" <<endl;
    getrusage(RUSAGE_SELF, &start);
    for (int i = 0; i < 200000000; i+=2) {
        start_index = tiny_query[i];
        end_index = tiny_query[i + 1];
        int res = approximateMode_BKMT->findQueryResult(start_index, end_index);
        result[(i / 2)] = res;
    }
    getrusage(RUSAGE_SELF, &end);
    cout << " Tiny Query Total Time : " << (double)(diffUserTime(&start, &end) + diffSystemTime(&start, &end)) <<" s " << endl;

    cout << "Med Query is:" <<endl;
    getrusage(RUSAGE_SELF, &start);
    for (int i = 200000000; i < 400000000; i+=2) {
        start_index = med_query[i - 200000000];
        end_index = med_query[i + 1 - 200000000];
        int res = approximateMode_BKMT->findQueryResult(start_index, end_index);
        result[(i / 2)] = res;
    }
    getrusage(RUSAGE_SELF, &end);
    cout << " Med Query Total Time : " << (double)(diffUserTime(&start, &end) + diffSystemTime(&start, &end)) <<" s " << endl;

    cout << "Huge Query is:" <<endl;
    getrusage(RUSAGE_SELF, &start);
    for (int i = 400000000; i < 600000000; i+=2) {
        start_index = huge_query[i - 400000000];
        end_index = huge_query[i + 1 - 400000000];
        int res = approximateMode_BKMT->findQueryResult(start_index, end_index);
        result[(i / 2)] = res;
    }
    getrusage(RUSAGE_SELF, &end);
    cout << " Huge Query Total Time : " << (double)(diffUserTime(&start, &end) + diffSystemTime(&start, &end)) <<" s " << endl;






    for (int i = 0; i < 200000000; i+=2) {
        start_index = tiny_query[i];
        end_index = tiny_query[i + 1];
        int row = result[i / 2];
        int left = leftsideBinarySearch(start_index, row - 1, Qa[row - 1], QaSize[row - 1]);
        int right = rightsideBinarySearch(end_index, row - 1, Qa[row - 1], QaSize[row - 1]);
        int freq = 0;
        if (left == -1 || right == -1) {
            freq = 0;
        } else {
            freq = right - left + 1;
        }

        result[i / 2] = freq;
    }

    for (int i = 200000000; i < 400000000; i+=2) {
        start_index = med_query[i - 200000000];
        end_index = med_query[i + 1 - 200000000];
        int row = result[i / 2];
        int left = leftsideBinarySearch(start_index, row - 1, Qa[row - 1], QaSize[row - 1]);
        int right = rightsideBinarySearch(end_index, row - 1, Qa[row - 1], QaSize[row - 1]);
        int freq = 0;
        if (left == -1 || right == -1) {
            freq = 0;
        } else {
            freq = right - left + 1;
        }

        result[i / 2] = freq;
    }

    for (int i = 400000000; i < 600000000; i+=2) {
        start_index = huge_query[i - 400000000];
        end_index = huge_query[i + 1 - 400000000];
        int row = result[i / 2];
        int left = leftsideBinarySearch(start_index, row - 1, Qa[row - 1], QaSize[row - 1]);
        int right = rightsideBinarySearch(end_index, row - 1, Qa[row - 1], QaSize[row - 1]);
        int freq = 0;
        if (left == -1 || right == -1) {
            freq = 0;
        } else {
            freq = right - left + 1;
        }

        result[i / 2] = freq;
    }



    //ofstream integrated_result("/users/grad/liu1/PSTMode.txt");
    ofstream integrated_result("/users/grad/liu1/data_text/PSTnvMode.txt");
    for (int i = 0; i < 300000000; i++) {
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


    delete[] tiny_query;
    delete[] med_query;
    delete[] huge_query;

}


void generate_query(int *tiny_query, int *med_query, int *huge_query, int query){
    int len = query;
    /*string tiny_file = "/users/grad/liu1/tiny.txt";
    string med_file = "/users/grad/liu1/med.txt";
    string huge_file = "/users/grad/liu1/huge.txt";*/
    string tiny_file = "/users/grad/liu1/data_text/tinyApp.txt";
    string med_file = "/users/grad/liu1/data_text/medApp.txt";
    string huge_file = "/users/grad/liu1/data_text/hugeApp.txt";
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
int leftsideBinarySearch(int start, int row, int* rowQa, int size) { // find the index of Qa[i] which is greater or equal than the left bound
    int left = 0, right = size - 1;
    while (left < right - 1) {
        int mid = left + (right - left) / 2;
        if (rowQa[mid] < start) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }
    if (rowQa[left] >= start) {
        return left;
    }
    if (rowQa[right] >= start) {
        return right;
    }
    return -1;
}
int rightsideBinarySearch(int end, int row, int* rowQa, int size) { // find the index of Qa[i] which is less or equal than the right bound
    int left = 0, right = size - 1;
    while (left < right - 1) {
        int mid = left + (right - left) / 2;
        if (rowQa[mid] > end) {
            right = mid - 1;
        } else {
            left = mid;
        }
    }
    if (rowQa[right] <= end) {
        return right;
    }
    if (rowQa[left] <= end) {
        return left;
    }
    return -1;
}
void read_Qa_file(int** Qa, int* QaSize, string file, int delta) {
    ifstream file_in(file);
    if (!file_in.is_open()) {
        cout << " cannot open the file" << endl;
    }
    for (int i = 0; i < delta; i++) {
        for (int j = 0; j < QaSize[i]; j++) {
            file_in >> Qa[i][j];
        }
    }

    file_in.close();
    file_in.clear();
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
