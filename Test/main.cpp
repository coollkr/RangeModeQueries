/*
 * Test program for verifying the correctness of approximate queries
 */

#include <iostream>
#include <algorithm>
#include <fstream>
#include<string>
#include <sys/resource.h>

using namespace std;
void read_file(double pa[], string file, int len);

int main() {


    int normal_length = 300000000;
    int oneplus_length = 300000000;

    // string file_in = "/users/grad/liu1/normal_result.txt";
    string file_in = "/users/grad/liu1/data_book/first_result_book_1000.txt";
    double *normal = new double[normal_length];
    read_file(normal, file_in, normal_length);

    string oneplus_file_in = "/users/grad/liu1/data_book/PSTnvMode.txt";
   //   string oneplus_file_in = "/users/grad/liu1/data_park/PArrayMode.txt";
   //   string oneplus_file_in = "/users/grad/liu1/data_park/PSTMode.txt";
    //  string oneplus_file_in =  "/users/grad/liu1/data_park/1plusBitMode.txt";
  //   string oneplus_file_in =  "/users/grad/liu1/data_park/1plusMode.txt";
   //   string oneplus_file_in = "/users/grad/liu1/data_park/TriMode.txt";
   //  string oneplus_file_in = "/users/grad/liu1/data_park/HeplusBitMode.txt";
  //   string oneplus_file_in = "/users/grad/liu1/data_book/HeplusMode.txt";

    double *oneplus = new double[oneplus_length];
    read_file(oneplus, oneplus_file_in, oneplus_length);
    double avgTiny = 0, avgMed = 0, avgHuge = 0;
    double sumTiny = 0, sumMed = 0, sumHuge = 0;
    double minTiny = 100, minMed = 100, minHuge = 100;
    double maxTiny = 0, maxMed = 0, maxHuge = 0;
    int diff = 0, zero = 0, error = 0;

    for (int i = 0; i < normal_length; i++) {

        if (normal[i] != oneplus[i] && oneplus[i] != 0) {
            diff++;
        }
        if (oneplus[i] == 0) {
            cout <<  i << "  " << normal[i]  << "   " << oneplus[i] << endl;
            zero++;
            continue;
        }
        if ((double)(normal[i] / oneplus[i]) < 1) {
            error++;
        }
        if ((double)(normal[i] / oneplus[i]) > 1.5) {
            error++;
            cout <<  i << "  " << normal[i]  << "   " << oneplus[i] << endl;
            cout << (double)(normal[i] / oneplus[i]) << endl;
        }

        if (0 <= i && i < 100000000) {
            sumTiny += (double)(normal[i] / oneplus[i]);
            minTiny = min(minTiny, (double)(normal[i] / oneplus[i]));
            maxTiny = max(maxTiny, (double)(normal[i] / oneplus[i]));
        } else if (i < 200000000) {
            sumMed += (double)(normal[i] / oneplus[i]);
            minMed = min(minMed, (double)(normal[i] / oneplus[i]));
            maxMed = max(maxMed, (double)(normal[i] / oneplus[i]));
        } else {
            sumHuge += (double)(normal[i] / oneplus[i]);
            minHuge = min(minHuge, (double)(normal[i] / oneplus[i]));
            maxHuge = max(maxHuge, (double)(normal[i] / oneplus[i]));
        }
    }


    cout << "zero: " << zero << endl;
    cout << "diff: " << diff << endl;
    cout << "error: " << error << endl;

    avgTiny = (double) (sumTiny / (100000000));
    cout << "avgTiny: " << avgTiny << endl;

    avgMed = (double) (sumMed / (100000000));
    cout << "avgMed: " << avgMed << endl;

    avgHuge = (double) (sumHuge / (100000000));
    cout << "avgHuge: " << avgHuge << endl;
    cout << "minTiny: " << minTiny << endl;
    cout << "minMed: " << minMed << endl;
    cout << "minHuge: " << minHuge << endl <<endl;

    cout << "maxTiny: " << maxTiny << endl;
    cout << "maxMed: " << maxMed << endl;
    cout << "maxHuge: " << maxHuge << endl;
    delete[] normal;
    delete[] oneplus;

}


void read_file(double pa[], string file, int len) {
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
