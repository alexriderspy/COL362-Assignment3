#include<stdio.h>
#include <stdlib.h>
#include <cstring>
#include <string>
#include <cmath>
#include <iostream>
#include<vector>

#include<bits/stdc++.h>
using namespace std;

const int MAX_CHAR = 256;

//#define M (1<<21)
//#define Mb (1<<30)
#define M 10
#define Mb 100

string sorted_arr[Mb];
int sz;

struct Trie {
	// index is set when node is a leaf
	// node, otherwise -1;
	int index;

	Trie* child[MAX_CHAR];

	/*to make new trie*/
	Trie()
	{
		for (int i = 0; i < MAX_CHAR; i++)
			child[i] = NULL;
		index = -1;
	}
};

/* function to insert in trie */
void insert(Trie* root, string str, int index)
{
	Trie* node = root;

	for (int i = 0; i < str.size(); i++) {

		/* taking ascii value to find index of
		child node */
		char ind = str[i] - 'a';

		/* making new path if not already */
		if (!node->child[ind])
			node->child[ind] = new Trie();

		// go to next node
		node = node->child[ind];
	}

	// Mark leaf (end of word) and store
	// index of word in arr[]
	node->index = index;
}

/* function for preorder traversal */
bool preorder(Trie* node, vector<string> arr)
{
	if (node == NULL)
		return false;

	for (int i = 0; i < MAX_CHAR; i++) {
		if (node->child[i] != NULL) {

			/* if leaf node then print key*/
			if (node->child[i]->index != -1){
				sorted_arr[sz++] = arr[node->child[i]->index];
            }

			preorder(node->child[i], arr);
		}
	}
    return false;
}

void sort(vector<string> arr){

    Trie* root = new Trie();
    int size = arr.size();    
	for (int i = 0; i < size; i++){
		insert(root, arr[i], i);
        sorted_arr[i] = '\0';
    }
    
    sz = 0;
    
    preorder(root, arr);
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

    long bufferSize = Mb;
    //cout<<bufferSize<<'\n';
    char* buffer = new char[bufferSize];
    
    //while stage
    int stage = 0;
    int num_runs = 1;

    // int stop = 0;
    // if (bufferSize < key_count) {
    //     bufferSize = key_count;
    //     stop = 1;
    // }
    // if (stop == 1){
    //     //continue;
    // }

    //Step 1. Make and store sorted runs
    vector<string>arr;
    char* out;
    int cnt = 0;
    while(fgets(buffer, bufferSize, ptr) != NULL) {
        cout<<buffer;

        int len = sizeof(buffer)/sizeof(char)-1;
        if (cnt + len <= bufferSize){
            arr.push_back(buffer);
            cnt += len;
        }
        else
        {
            sort(arr);
            cnt = 0;
            FILE* ptr_new;
            string s = "temp.0." + to_string(num_runs);
            char* char_array = new char[s.length() + 1];
            strcpy(char_array, s.c_str());
            ptr_new = fopen(char_array, "w");
            string fin = "";
            for (int i=0;i<arr.size();++i){
                fin += sorted_arr[i];
            }
            char *a = new char[fin.size()];
            strcpy(a, fin.c_str());
            fputs(a, ptr_new);        
            num_runs++;

            arr.clear();
            arr.push_back(buffer);
        }
    }

    if (arr.size()!=0){
        FILE* ptr_new;
        string s = "temp.0." + to_string(num_runs);
        char* char_array = new char[s.length() + 1];
        strcpy(char_array, s.c_str());
        ptr_new = fopen(char_array, "w");
        string fin = "";
        for (int i=0;i<arr.size();++i){
            fin += sorted_arr[i];
        }
        char *a = new char[fin.size()+1];
        strcpy(a, fin.c_str());
        fputs(a, ptr_new);        
        num_runs++;
    
    }
    //Step 2. Merge the sorted runs.
    int temp = ceil(num_runs/(M-1));
    for(int i = 0;i<temp;i++)
    {

    }

    fclose(ptr);
    return 0;
}


int main(){
    string s = "random1.list";
    string out = "out.txt";
    char* char_array = new char[s.length() + 1];
    char* char_array2 = new char[out.length() + 1];
    strcpy(char_array, s.c_str());
    strcpy(char_array2, out.c_str());
    external_merge_sort_withstop(char_array, char_array2, 3, 2, 0);
    return 0;
}
