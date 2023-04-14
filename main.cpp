#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <string>
#include <cmath>
#include <iostream>
#include <vector>
#include <map>
#include <queue>
#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>

using namespace std;

const int MAX_CHAR = 256;

#define M 10
#define Mb (1 << 30)

#define bufferSize 1025
#define pq_type priority_queue<pair<string, int>, vector<pair<string, int>>, greater<pair<string, int>>>

int Lb;
vector<int> return_type;

int fetch(vector<string> &a, int start_cnt, int i, string fname)
{
    char *buffer = new char[bufferSize];
    int cnt = 0;
    int ind_cnt = 0;
    int flag = 0;
    a.clear();

    fstream myFile(fname, ios::in);
    myFile.seekg(start_cnt, ios::beg);

    // fseek
    string s1;
    while (myFile)
    {
        getline(myFile, s1);
        cnt += s1.length();
        if (cnt <= Lb)
            (a).push_back(s1);
        else
        {
            flag = 1;
            break; // ind will not be incremented, this string will be read again next time
        }
        ind_cnt = cnt;
    }
    myFile.close();
    if (flag == 0)
    {
        return_type.push_back(i);
    }
    return ind_cnt; // the next start index
}

void push_pq(pq_type &pq, string st, int i)
{
    pair<string, int> temp;
    temp.first = st;
    temp.second = i;
    pq.push(temp);
}

void write_to_file(string fname, vector<string> &content, int mode)
{
    fstream myFile;
    if (mode == 1){
        myFile.open(fname, ios::app);
    }else{
        myFile.open(fname, ios::out);
    }

    string fin = "";
    for (int i = 0; i < content.size(); ++i)
    {
        fin += content[i];
    }

    myFile << fin;
    myFile.close();
}

void merge(int ind1, int ind2, int stage, int num, const char *output, int wr)
{
    // ind1 and ind2 are inclusive. We are going to read the runs stored in these files from ind1 to ind2 and merge them

    return_type.clear();
    vector<vector<string>> inputs;
    vector<string> temp, output_buffer;
    vector<int> pointers, lengths, indices;
    vector<string> filenames;
    pq_type pq;
    int mode = 0;

    for (int i = 0; i <= ind2 - ind1; i++)
    {
        string s = "temp." + to_string(stage - 1) + "." + to_string(i + ind1 + 1);

        filenames.push_back(s);

        indices.push_back(fetch(temp, 0, i, s));

        inputs.push_back(temp);
        pointers.push_back(0);
        lengths.push_back(inputs[i].size());

        // Initialising the pq - writing the first 'x' elements into the priority queue
        push_pq(pq, inputs[i][0], i);
    }
    int num_active = inputs.size();

    // The main merge step

    // need to fetch Lb bytes after each run that finishes

    // stop the loop when every character of every file has been read and merged
    int char_cnt = 0;
    while (num_active > 0)
    {
        int ind = pq.top().second;
        string to_write = pq.top().first;
        if (char_cnt + to_write.length() > Lb)
        {
            // write to file
            string fname = "temp." + to_string(stage) + "." + to_string(num);
            if (wr == 1)
                fname = string(output);
            int n = output_buffer.size();
            write_to_file(fname, output_buffer, mode);
            mode = 1;
            output_buffer.clear();
            char_cnt = 0;
        }
        output_buffer.push_back(to_write);
        char_cnt += to_write.length();
        pq.pop();
        pointers[ind] += 1;

        if (pointers[ind] < lengths[ind])
            push_pq(pq, inputs[ind][pointers[ind]], ind);
        else
        {
            // fetch the next L characters from the file
            // if file complete then reduce num_active files by 1
            if (find(return_type.begin(), return_type.end(), ind) != return_type.end())
            {
                num_active -= 1;
            }
            else
            {
                string s = "temp." + to_string(stage - 1) + "." + to_string(ind + ind1 + 1);
                
                int next_ind = fetch((inputs[ind]), indices[ind], ind, filenames[ind]);
                pointers[ind] = 0;
                lengths[ind] = inputs[ind].size();
                indices[ind] = next_ind;
                push_pq(pq, inputs[ind][pointers[ind]], ind);
            }
        }

        if (pq.size() == 0)
        {
            string fname = "temp." + to_string(stage) + "." + to_string(num);
            if (wr == 1)
                fname = string(output);
            int n = output_buffer.size();
            write_to_file(fname, output_buffer, mode);
            break;
        }
    }
}

int sort_and_store(vector<string> &arr, int num_runs)
{
    sort(arr.begin(),arr.end());
    string fname = "temp.0." + to_string(num_runs);
    write_to_file(fname, arr, 0);
    arr.clear();
    return num_runs + 1;
}

int external_merge_sort_withstop(const char *input, const char *output, const long key_count, const int k = 2, const int num_merges = 0)
{
    FILE *ptr;
    ptr = fopen(input, "r");
    char *buffer = new char[bufferSize];
    int stage, cnt = 0;
    int num_runs = 1;
    vector<string> arr;
    char *out;

    Lb = Mb / (k + 1);
    int total_keys=0;
    // Step 1. Make and store sorted runs
    while (fgets(buffer, bufferSize, ptr) != NULL)
    {
        // cnt stores the the sum of lenghs SUPPOSING the new string is added to the list
        string s1(buffer);
        cnt += s1.length();
        if (cnt > Mb)
        {
            cnt = 0;
            num_runs = sort_and_store(arr, num_runs);
        }
        arr.push_back(buffer);
        total_keys++;
        if(total_keys == key_count)
            break;
    }
    if (arr.size() != 0)
        num_runs = sort_and_store(arr, num_runs);

    fclose(ptr);

    int total_runs = num_runs;
    // Step 2. Merge the sorted runs.
    num_runs--;
    stage = 1;
    if (num_runs == 1)
    {
        ifstream src("temp.0.1", std::ios::binary);
        ofstream dst(string(output), std::ios::binary);
        dst << src.rdbuf();
    }
    int total_merges = 0;
    while (num_runs > 1)
    {
        int ind1 = 0;
        int ind2 = -1;
        int cnt = 0;
        int wr = 0;
        while (ind2 < num_runs - 1)
        {
            ind1 = ind2 + 1;
            ind2 = min(ind1 + k, num_runs) - 1;
            if (ind2 >= num_runs - 1 && cnt == 0)
                wr = 1;
            merge(ind1, ind2, stage, cnt + 1, output, wr);
            cnt++;
            total_merges++;
            if (total_merges == num_merges)
                break;
        }
        if (total_merges == num_merges)
            break;
        num_runs = cnt;
        stage++;
    }

    return 0;
}

int main()
{   
    string s = "random1.list";
    string out = "out.txt";
    char *file_name = new char[s.length() + 1];
    char *file_name2 = new char[out.length() + 1];
    strcpy(file_name, s.c_str());
    strcpy(file_name2, out.c_str());
    auto start = chrono::steady_clock::now();
    external_merge_sort_withstop(file_name, file_name2, 1000000000, 2, 0);
    auto end = chrono::steady_clock::now();

    cout << "Elapsed time in seconds: " << chrono::duration_cast<chrono::seconds>(end - start).count() << " sec";
    return 0;
}
