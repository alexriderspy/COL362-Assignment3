#include<stdio.h>
#include <stdlib.h>
#include <string>
#include <cmath>
#include <iostream>

using namespace std;

//Uncomment everything as and when required, this is just the basic template

void sort(char* buffer, int bufferSize){
//implement sort
    return;
}

void merge(int ind1, int ind2, int stage){
    // ind1 and ind2 are inclusive. We are going to read the runs stored in these files from ind1 to ind2 and merge them

    // FILE* ptr;
    // string s = "temp." + to_string(stage) + "." + to_string(num_runs);
    // char* char_array = new char[s.length() + 1];
    // strcpy(char_array, s.c_str());
    // ptr = fopen(input, "r");
}

int external_merge_sort_withstop ( const char* input , const char* output , const long key_count , const int k = 2 , const int num_merges = 0 ){

    FILE* ptr;
    ptr = fopen(input, "r");

    long bufferSize = 1024; //10^6
    char* buffer = new char[bufferSize];
    
    //while stage
    int stage = 0;
    int num_runs = 0;

    // int stop = 0;
    // if (bufferSize < key_count) {
    //     bufferSize = key_count;
    //     stop = 1;
    // }
    // if (stop == 1){
    //     //continue;
    // }

    //Step 1. Make and store sorted runs
    while(fgets(buffer, bufferSize, ptr) != NULL) {
        cout<<buffer<<endl;
        // sort(buffer, bufferSize);
        
        //buffer has the sorted values
        // FILE* ptr_new;
        // string s = "temp." + to_string(stage) + "." + to_string(num_runs);
        // char* char_array = new char[s.length() + 1];
        // strcpy(char_array, s.c_str());
        // ptr_new = fopen(char_array, "w");
        // fputs(buffer, ptr_new);        
        // num_runs++;
    }

    //Step 2. Merge the sorted runs.
    int M = bufferSize;
    int temp = ceil(num_runs/(M-1));
    for(int i = 0;i<temp;i++)
    {

    }

    fclose(ptr);
    return 0;
}


int main(){
    string s = "english.list";
    string out = "out.txt";
    char* char_array = new char[s.length() + 1];
    char* char_array2 = new char[out.length() + 1];
    strcpy(char_array, s.c_str());
    strcpy(char_array2, out.c_str());
    external_merge_sort_withstop(char_array, char_array2, 3, 2, 0);
    return 0;
}