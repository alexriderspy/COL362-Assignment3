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
#include <set>

using namespace std;

const int MAX_CHAR = 256;

#define bufferSize 1026
#define pq_type priority_queue<pair<string, long long int>, vector<pair<string, long long int>>, greater<pair<string, long long int>>>

long long int Lb_input, Lb_output;

// long long int Mb = ((long long)(1 << 28)*11)/10; //k = 2
long long int Mb = (long long)(1 << 27); //k = 8
// long long int Mb = (long long)(1 << 27); //k = 16



//long long int Mb = (1<<7);
set<int> return_type;

long long int fetch(vector<string> &a, long long int start_cnt, int i, string &fname)
{
    long long int cnt = 0;
    long long int ind_cnt = start_cnt;
    int flag = 0;
    a.clear();

    fstream myFile(fname, ios::in);
    if (!myFile)
    {
        return -1;
    }
    myFile.seekg(start_cnt, ios::beg);

    // fseek
    string s1;

    while (!myFile.eof())
    {
        getline(myFile, s1);
        // cout<<s1<<'\n';
        if (myFile.eof())
        {
            // cout<<"in\n";
            break;
        }
        // cout<<Lb<<'\n';
        cnt += (long long)s1.length();
        ind_cnt += (long long)s1.length() + 1;
        if (cnt <= Lb_input)
        {
            a.push_back(s1);
            // cout<<cnt<<'\n';
        }
        else
        {
            flag = 1;
            ind_cnt = ind_cnt - (long long)s1.length() - 1;
            break; // ind will not be incremented, this string will be read again next time
        }
    }

    // cout<<"end\n";
    myFile.close();
    if (flag == 0)
    {
        return_type.insert(i);
    }
    // cout<<ind_cnt<<'\n';
    return ind_cnt; // the next start index
}

int write_to_file(string fname, const vector<string> &content, int mode)
{
    fstream myFile;
    if (mode == 1)
    {
        myFile.open(fname, ios::app);
    }
    else
    {
        myFile.open(fname, ios::out);
    }

    if (!myFile)
    {
        return -1;
    }
    string fin = "";
    for (int i = 0; i < content.size(); ++i)
    {
        // content[i] = content[i]+'\n';
        fin += content[i] + '\n';
    }

    myFile << fin;
    myFile.close();
    return 0;
}

int merge(long long int ind1, long long int ind2, int stage, long long int num, const char *output, int wr)
{
    // ind1 and ind2 are inclusive. We are going to read the runs stored in these files from ind1 to ind2 and merge them
    return_type.clear();
    vector<vector<string>> inputs;
    vector<string> temp, output_buffer;
    vector<long long int> pointers, lengths, indices;
    vector<string> filenames;
    pq_type pq;
    int mode = 0;

    for (long long int i = 0; i <= ind2 - ind1; i++)
    {
        string s = "temp." + to_string(stage - 1) + "." + to_string(i + ind1 + 1);
        // cout<<s<<'\n';
        filenames.push_back(s);
        // cout<<"before1\n";
        long long int val = fetch(temp, 0, i, s);
        if (val == -1)
        {
            return -1;
        }
        indices.push_back(val);
        // cout<<"after1\n"<<temp.size();
        inputs.push_back(temp);
        pointers.push_back(0);
        lengths.push_back(inputs[i].size());

        // Initialising the pq - writing the first 'x' elements into the priority queue
        pq.push({inputs[i][0], i});
    }

    long long int num_active = inputs.size();

    // cout<<num_active<<'\n';
    //  The main merge step

    // need to fetch Lb bytes after each run that finishes

    // stop the loop when every character of every file has been read and merged
    long long int char_cnt = 0;

    while (num_active > 0)
    {
        // cout<<num_active<<'\n';
        long long int ind = pq.top().second;
        string to_write = pq.top().first;
        if (char_cnt + (long long)to_write.length() > Lb_output)
        {
            // write to file
            string fname = "temp." + to_string(stage) + "." + to_string(num);
            if (wr == 1)
                fname = string(output);
            long long int n = output_buffer.size();
            int ret = write_to_file(fname, output_buffer, mode);
            if (ret == -1)
            {
                return -1;
            }
            mode = 1;
            output_buffer.clear();
            char_cnt = 0;
        }
        output_buffer.push_back(to_write);
        char_cnt += (long long)to_write.length();
        pq.pop();
        pointers[ind] += 1;

        if (pointers[ind] < lengths[ind])
            pq.push({inputs[ind][pointers[ind]], ind});
        else
        {
            // fetch the next L characters from the file
            // if file complete then reduce num_active files by 1
            if (return_type.find(ind) != return_type.end())
            {
                // cout<<"eof"<<endl;
                num_active -= 1;
            }
            else
            {
                string s = "temp." + to_string(stage - 1) + "." + to_string(ind + ind1 + 1);
                // cout<<indices[ind]<<endl;
                long long int next_ind = fetch((inputs[ind]), indices[ind], ind, filenames[ind]);
                if (next_ind == -1)
                {
                    return -1;
                }
                pointers[ind] = 0;
                lengths[ind] = inputs[ind].size();
                indices[ind] = next_ind;
                // cout<<inputs[ind][pointers[ind]]<<endl;
                pq.push({inputs[ind][pointers[ind]], ind});
            }
        }

        if (pq.size() == 0)
        {
            string fname = "temp." + to_string(stage) + "." + to_string(num);
            if (wr == 1)
                fname = string(output);
            long long int n = output_buffer.size();
            int ret = write_to_file(fname, output_buffer, mode);
            if (ret == -1)
            {
                return -1;
            }
            break;
        }
    }
    return 0;
}

long long int sort_and_store(vector<string> &arr, long long int num_runs)
{
    sort(arr.begin(), arr.end());
    string fname = "temp.0." + to_string(num_runs);
    int ret = write_to_file(fname, arr, 0);
    if (ret == -1)
    {
        return -1;
    }
    arr.clear();
    return num_runs + 1;
}

int external_merge_sort_withstop(const char *input, const char *output, const long key_count, const int k = 2, const int num_merges = 0)
{
    fstream myFile(input, ios::in);
    if (!myFile)
    {
        return -1;
    }

    string s1;
    int stage;
    long long int cnt = 0;
    long long int num_runs = 1;
    vector<string> arr;
    char *out;
    long long int x_num = 11;
    long long int x_den = 6;

    if(k==2)
        Mb = ((long long)(1 << 28)*11)/10;
    else if(k==16)
        Mb = (long long)(1 << 27);
    else
        Mb = (long long)(1 << 27);


    long total_keys = 0;
    // Step 1. Make and store sorted runs
    while (!myFile.eof())
    {
        // cnt stores the the sum of lenghs SUPPOSING the new string is added to the list
        getline(myFile, s1);
        // cout<<s1<<'\n';

        if (myFile.eof())
        {
            break;
        }
        cnt += (long long)s1.length() + 1;
        if (cnt > Mb)
        {
            cnt = 0;
            num_runs = sort_and_store(arr, num_runs);
            if (num_runs == -1)
            {
                return -1;
            }
        }
        arr.push_back(s1);
        total_keys++;
        if (total_keys == key_count)
            break;
    }
    // cout<<"end\n";
    if (arr.size() != 0)
    {
        num_runs = sort_and_store(arr, num_runs);
        if (num_runs == -1)
        {
            return -1;
        }
    }
    long long int total_runs = num_runs;
    // Step 2. Merge the sorted runs.
    num_runs--;
    stage = 1;
    if (num_runs == 1)
    {
        ifstream src("temp.0.1", ios::binary);
        ofstream dst(string(output), ios::binary);
        dst << src.rdbuf();
    }
    long long int total_merges = 0;
    while (num_runs > 1)
    {
        long long int ind1 = 0;
        long long int ind2 = -1;
        long long int cnt = 0;
        int wr = 0;
        while (ind2 < num_runs - 1)
        {
            ind1 = ind2 + 1;
            ind2 = min(ind1 + (long long)k, num_runs) - 1;
            if (ind2 >= num_runs - 1 && cnt == 0)
                wr = 1;
            // cout<<ind1<<' '<<ind2<<'\n';
            long long k1 = ind2 - ind1 + 1;
            Lb_input = max(Mb * x_den / (k * x_den + x_num), (long long)bufferSize);
            Lb_output = max(((x_num * Lb_input) / x_den), (long long)bufferSize);

            int ret = merge(ind1, ind2, stage, cnt + 1, output, wr);
            if (ret == -1)
            {
                return -1;
            }
            // cout<<"after\n";
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

    return total_merges;
}

int main()
{
    string s = "big_random.txt";
    string out = "out.txt";
    char *file_name = new char[s.length() + 1];
    char *file_name2 = new char[out.length() + 1];
    strcpy(file_name, s.c_str());
    strcpy(file_name2, out.c_str());
    auto start = chrono::steady_clock::now();
    int ans = external_merge_sort_withstop(file_name, file_name2, 1000000000, 16, 0);
    auto end = chrono::steady_clock::now();
    cout << ans << '\n';
    cout << "Elapsed time in milliseconds: " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << " millisec";
    return 0;
}
