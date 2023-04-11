#include<stdio.h>
#include <stdlib.h>
#include <string>

using namespace std;

void sort(char* buffer, int bufferSize){
//implement sort
    return;
}

int external_merge_sort_withstop ( const char* input , const char* output , const long key_count , const int k = 2 , const int num_merges = 0 ){

    FILE* ptr;
    ptr = fopen(input, "r");

    long bufferSize = 1024 * 1024; //10^6
    char* buffer = new char[bufferSize];
    
    //while stage
    int stage = 1;
    int num_runs = 0;

    int stop = 0;
    if (bufferSize < key_count) {
        bufferSize = key_count;
        stop = 1;
    }
    if (stop == 1){
        //continue;
    }

    while(fgets(buffer, bufferSize, ptr) != NULL) {
        sort(buffer, bufferSize);
        //buffer has the sorted values
        FILE* ptr_new;
        string s = "temp." + to_string(stage) + "." + to_string(num_runs);
        char* char_array = new char[s.length() + 1];
        strcpy(char_array, s.c_str());
        ptr_new = fopen(char_array, "r");
        fputs(buffer, ptr_new);        
        num_runs++;
    }

    fclose(ptr);
    return 0;
}


int main(){
    return 0;
}