/*
 * It is the main.cpp file to run the nv^1 data structure
 */

#include <iostream>
#include <algorithm>
#include <fstream>
#include <sys/resource.h>

using namespace std;

double diffUserTime(struct rusage *start, struct rusage *end);
double diffSystemTime(struct rusage *start, struct rusage *end);
void read_file(int pa[], string file, int len);
void generate_query(int *tiny_query, int *med_query, int *huge_query, int query);



int main() {


    int len = 0;
    cout << "please input the length of the array: ";
    //cin >> len;
    len = 10000000;
    //  len = 8571089;
    //  len = 6715122;
    //   len = 100000;
    int *pa = new int[len];
    int *pa_copy = new int[len];

    //  string file_in = "/users/grad/liu1/data_fly/fly_1000.txt";
    //   string file_in = "/users/grad/liu1/data_book/book_1000.txt";
    //    string file_in = "/users/grad/liu1/data_ip/ip_857.txt";
    //   string file_in = "/users/grad/liu1/data_text/text_671.txt";
    //    string file_in = "/users/grad/liu1/data_library/lib_1000.txt";
    //  string file_in = "/users/grad/liu1/book1.txt";
    string file_in = "/users/grad/liu1/data_park/park_1000.txt";

    read_file(pa, file_in, len);
    int *tiny_query = new int[2000000];
    int *med_query = new int[2000000];
    int *huge_query = new int[2000000];
    int *result = new int[3000000];
    generate_query(tiny_query, med_query, huge_query, 2000000);
    // nv1: just sort the query range and traverse once to get the mode

    int mode = 0;    // save mode
    int maxFreq = 0;   // save the freq of the mode
    int start_index = 0;
    int end_index = 0;
    struct rusage start, end;

    cout << "Tiny Query is:" <<endl;
    getrusage(RUSAGE_SELF, &start);
    for (int i = 0; i < 2000000; i+=2) {
        start_index = tiny_query[i];
        end_index = tiny_query[i + 1];
        mode = 0;
        maxFreq = 0;
        for (int i = start_index - 1; i < end_index; i++) {
            pa_copy[i] = pa[i];
        }
        //cout << "i/2: " << i / 2 << "  start: " << start_index << "  end:  " << end_index << endl;

        sort(pa_copy + start_index - 1, pa_copy + end_index);

        int count = 0;

        for (int i = start_index - 1; i < end_index; i++) {
            if (i + 1 < end_index && pa_copy[i] == pa_copy[i + 1]) {
                count++;
            } else {
                count = count + 1;
                if (count > maxFreq) {
                    maxFreq = count;
                    mode = pa_copy[i];
                }
                count = 0;
            }
        }

        result[(i / 2)] = maxFreq;
    }
    getrusage(RUSAGE_SELF, &end);
    cout << " Tiny Query Total Time : " << (double)(diffUserTime(&start, &end) + diffSystemTime(&start, &end)) <<" s " << endl;

    cout << "Med Query is:" <<endl;
    getrusage(RUSAGE_SELF, &start);
    for (int i = 2000000; i < 4000000; i+=2) {
        start_index = med_query[i - 2000000];
        end_index = med_query[i + 1 - 2000000];
        mode = 0;
        maxFreq = 0;
        for (int i = start_index - 1; i < end_index; i++) {
            pa_copy[i] = pa[i];
        }
        //cout << "i/2: " << i / 2 << "  start: " << start_index << "  end:  " << end_index << endl;

        sort(pa_copy + start_index - 1, pa_copy + end_index);

        int count = 0;

        for (int i = start_index - 1; i < end_index; i++) {
            if (i + 1 < end_index && pa_copy[i] == pa_copy[i + 1]) {
                count++;
            } else {
                count = count + 1;
                if (count > maxFreq) {
                    maxFreq = count;
                    mode = pa_copy[i];
                }
                count = 0;
            }
        }

        result[(i / 2)] = maxFreq;
    }
    getrusage(RUSAGE_SELF, &end);
    cout << " Med Query Total Time : " << (double)(diffUserTime(&start, &end) + diffSystemTime(&start, &end)) <<" s " << endl;

    cout << "Huge Query is:" <<endl;
    getrusage(RUSAGE_SELF, &start);
    for (int i = 4000000; i < 6000000; i+=2) {
        start_index = huge_query[i - 4000000];
        end_index = huge_query[i + 1 - 4000000];
        mode = 0;
        maxFreq = 0;
        for (int i = start_index - 1; i < end_index; i++) {
            pa_copy[i] = pa[i];
        }
        //cout << "i/2: " << i / 2 << "  start: " << start_index << "  end:  " << end_index << endl;

        sort(pa_copy + start_index - 1, pa_copy + end_index);

        int count = 0;

        for (int i = start_index - 1; i < end_index; i++) {
            if (i + 1 < end_index && pa_copy[i] == pa_copy[i + 1]) {
                count++;
            } else {
                count = count + 1;
                if (count > maxFreq) {
                    maxFreq = count;
                    mode = pa_copy[i];
                }
                count = 0;
            }
        }

        result[(i / 2)] = maxFreq;
    }
    getrusage(RUSAGE_SELF, &end);
    cout << " Huge Query Total Time : " << (double)(diffUserTime(&start, &end) + diffSystemTime(&start, &end)) <<" s " << endl;



    delete[] pa;
    delete[] pa_copy;
    delete[] tiny_query;
    delete[] med_query;
    delete[] huge_query;


    ofstream normal_result("/users/grad/liu1/data_park/2normal_result_1000.txt");
    for (int i = 0; i < 3000000; i++) {

        normal_result << result[i] << " ";
        if ((i + 1) % 10 == 0) {
            normal_result << endl;
        }
    }
    if (normal_result) {
        cout << endl;
        cout << "create normal result file successfully!" << endl;
    }
    normal_result.close();
    normal_result.clear();
    delete[] result;
}
void generate_query(int *tiny_query, int *med_query, int *huge_query, int query){
    int len = query;
    /*string tiny_file = "/users/grad/liu1/tiny.txt";
    string med_file = "/users/grad/liu1/med.txt";
    string huge_file = "/users/grad/liu1/huge.txt";*/
    string tiny_file = "/users/grad/liu1/data_park/tinyExact.txt";
    string med_file = "/users/grad/liu1/data_park/medExact.txt";
    string huge_file = "/users/grad/liu1/data_park/hugeExact.txt";
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

void read_file(int pa[], string file, int len) {
    ifstream file_in(file);
    if (!file_in.is_open()) {
        cout << " cannot open the file" << endl;
    }
    for (int i = 0; i < len; i++) {
        file_in >> pa[i];
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