#include<stdio.h>
#include <stdlib.h>
// #include <cstring>
#include <string>
#include <cmath>
#include <iostream>
#include<vector>
#include <map>
#include <queue>

using namespace std;

const int MAX_CHAR = 256;

//#define M (1<<21)
//#define Mb (1<<30)
#define M 10
#define Mb 100
#define L 10 //max blocks from one temp file in one merge step
#define Lb 100  //max characters from one temp file in one merge step
#define bufferSize 1024
#define pq_type priority_queue<pair<string, int>, vector<pair<string, int> >, greater<pair<string, int> > >

string sorted_arr[Mb];
vector<int> return_type;
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

int fetch(vector<string> *a, FILE * ptr, int start_ind, int i, string fname)
{
    char * buffer = new char[bufferSize];
    int cnt = 0;
    int ind = 0;
    int flag = 0;
    (*a).clear();
    char* file_name = new char[fname.length() + 1];
    strcpy(file_name, fname.c_str());
    // ptr =  fopen(file_name, "r");
    while(fgets(buffer, bufferSize, ptr) != NULL) 
    {
        if(ind>=start_ind)
        {
            string s1(buffer);
            cnt += s1.length();
            if(cnt<=Lb)
                (*a).push_back(buffer);
            else
            {
                flag = 1;
                break;      //ind will not be incremented, this string will be read again next time
            }
        }
        ind++;
    }
    fclose(ptr);
    if(flag==0)
    {
        return_type.push_back(i);
    }

    return ind; //the next start index
}

void push_pq(pq_type *pq, string st, int i)
{
    pair<string, int> temp;
    temp.first = st;
    temp.second = i;
    (*pq).push(temp);
}

void write_to_file(string fname, string *content, int n, int mode)
{
    FILE* ptr_new;
    char* file_name = new char[fname.length() + 1];
    strcpy(file_name, fname.c_str());
    const char *c = "w";
    if(mode==1)
        c = "a";
    ptr_new = fopen(file_name, c);
    string fin = "";
    for (int i=0;i<n;++i){
        fin += content[i];
    }
    char *a = new char[fin.size()+1];
    strcpy(a, fin.c_str());
    fputs(a, ptr_new);
    fclose(ptr_new);
}

void merge(int ind1, int ind2, int stage, int num){

    // ind1 and ind2 are inclusive. We are going to read the runs stored in these files from ind1 to ind2 and merge them

    return_type.clear();
    FILE * ptrs[ind2-ind1+1];
    vector<vector<string> > inputs;
    vector<string> temp, output_buffer;
    vector<int> pointers, lengths, indices;
    vector<string> filenames;
    pq_type pq;
    int mode = 0;
    
    for(int i = 0;i<=ind2-ind1;i++)
    {
        string s = "temp." + to_string(stage-1) + "." + to_string(i+ind1+1);
        filenames.push_back(s);
        char* file_name = new char[s.length() + 1];
        strcpy(file_name, s.c_str());
        ptrs[i] = fopen(file_name, "r");
        //fetching the first Lb bytes from the temp file, and storing the last index accessed + 1
        indices.push_back(fetch(&temp, ptrs[i], 0, i, s));   
        // fclose(ptrs[i]);
        inputs.push_back(temp);
        pointers.push_back(0);
        lengths.push_back(inputs[i].size());
        // Initialising the pq - writing the first 'x' elements into the priority queue
        push_pq(&pq, inputs[i][0], i);
    }
    int num_active = inputs.size();
        
    // The main merge step

    // need to fetch Lb bytes after each run that finishes
    
    //stop the loop when every character of every file has been read and merged
    int char_cnt = 0;
    while(num_active>0)
    {
        int ind = pq.top().second;
        string to_write = pq.top().first;
        if(char_cnt + to_write.length() > Lb)
        {
            //write to file
            string fname = "temp." + to_string(stage) + "." + to_string(num);
            int n = output_buffer.size();
            string output_strings[n];
            for(int i = 0;i<n;i++)
                output_strings[i] = output_buffer[i];
            write_to_file(fname, output_strings, n, mode);
            mode = 1;
            output_buffer.clear();
            char_cnt=0;
        }
        output_buffer.push_back(to_write);
        char_cnt+=to_write.length();
        pq.pop();
        pointers[ind]+=1;
        
        if(pointers[ind]<lengths[ind])
            push_pq(&pq, inputs[ind][pointers[ind]], ind);
        else
        {
            //fetch the next L characters from the file
            //if file complete then reduce num_active files by 1
            if(find(return_type.begin(), return_type.end(), ind) != return_type.end())
            {
                num_active-=1;
            }   
            else
            {
                string s = "temp." + to_string(stage-1) + "." + to_string(ind+ind1+1);
                char* file_name = new char[s.length() + 1];
                strcpy(file_name, s.c_str());
                ptrs[ind] = fopen(file_name, "r");
                int next_ind = fetch(&(inputs[ind]), ptrs[ind], indices[ind], ind, filenames[ind]);
                pointers[ind] = 0;
                lengths[ind] = inputs[ind].size();
                indices[ind] = next_ind;
                push_pq(&pq, inputs[ind][pointers[ind]], ind);
            }
        }

        if(pq.size()==0)
        {
            string fname = "temp." + to_string(stage) + "." + to_string(num);
            int n = output_buffer.size();
            string output_strings[n];
            for(int i = 0;i<n;i++)
                output_strings[i] = output_buffer[i];
            write_to_file(fname, output_strings, n, mode);
            break;
        }
    }
    
}

int sort_and_store(vector<string> *arr, int num_runs)
{
    sort(*arr);
    string fname = "temp.0." + to_string(num_runs);
    write_to_file(fname, sorted_arr, (*arr).size(), 0);
    (*arr).clear();   
    return num_runs+1;
}

int external_merge_sort_withstop ( const char* input , const char* output , const long key_count , const int k = 2 , const int num_merges = 0 ){

    FILE* ptr;
    ptr = fopen(input, "r");
    char* buffer = new char[bufferSize];
    int stage, cnt = 0;
    int num_runs = 1;
    vector<string>arr;
    char* out;

    // int stop = 0;
    // if (bufferSize < key_count) {
    //     bufferSize = key_count;
    //     stop = 1;
    // }
    // if (stop == 1){
    //     //continue;
    // }

    //Step 1. Make and store sorted runs
    while(fgets(buffer, bufferSize, ptr) != NULL) 
    {
        //cnt stores the the sum of lenghs SUPPOSING the new string is added to the list
        string s1(buffer);
        cnt += s1.length();  
        if (cnt > Mb)
        {
            cnt = 0;
            num_runs = sort_and_store(&arr, num_runs);
        }
        arr.push_back(buffer);
    }
    if (arr.size()!=0)
        num_runs = sort_and_store(&arr, num_runs);

    fclose(ptr);

    //Step 2. Merge the sorted runs.
    num_runs--;
    stage = 1;
    

    while(num_runs>1)
    {
        // int temp = ceil((num_runs)*1.0/(M-1));
        int ind1 = 0;
        int ind2 = -1;
        int cnt = 0;
        while(ind2<num_runs-1)
        {
            
            ind1 = ind2+1;
            ind2 = min(ind1 + min(M-1, k), num_runs) - 1;
            merge(ind1, ind2, stage, cnt+1);
            cnt++;
        }
        num_runs = cnt;
        stage++;
    }
    return 0;
}


int main(){
    string s = "english-subset.txt";
    string out = "out.txt";
    char* file_name = new char[s.length() + 1];
    char* file_name2 = new char[out.length() + 1];
    strcpy(file_name, s.c_str());
    strcpy(file_name2, out.c_str());
    external_merge_sort_withstop(file_name, file_name2, 3, 2, 0);
    return 0;
}