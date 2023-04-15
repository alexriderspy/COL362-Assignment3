#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <zlib.h>

using namespace std;

// Compresses a block of data using zlib
vector<char> compress_data(const char* data, size_t size)
{
    vector<char> compressed_data;
    const int compression_level = Z_DEFAULT_COMPRESSION;

    // Create zlib compression stream
    z_stream stream;
    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;
    deflateInit(&stream, compression_level);

    // Compress data
    const int buffer_size = 1024 * 1024;
    char buffer[buffer_size];
    stream.avail_in = size;
    stream.next_in = const_cast<Bytef*>(reinterpret_cast<const Bytef*>(data));
    do {
        stream.avail_out = buffer_size;
        stream.next_out = reinterpret_cast<Bytef*>(buffer);
        deflate(&stream, Z_FINISH);
        compressed_data.insert(compressed_data.end(), buffer, buffer + (buffer_size - stream.avail_out));
    } while (stream.avail_out == 0);

    // Clean up
    deflateEnd(&stream);
    return compressed_data;
}

// Decompresses a block of data using zlib
vector<char> decompress_data(const char* data, size_t size)
{
    vector<char> decompressed_data;

    // Create zlib decompression stream
    z_stream stream;
    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;
    inflateInit(&stream);

    // Decompress data
    const int buffer_size = 1024 * 1024;
    char buffer[buffer_size];
    stream.avail_in = size;
    stream.next_in = const_cast<Bytef*>(reinterpret_cast<const Bytef*>(data));
    do {
        stream.avail_out = buffer_size;
        stream.next_out = reinterpret_cast<Bytef*>(buffer);
        inflate(&stream, Z_NO_FLUSH);
        decompressed_data.insert(decompressed_data.end(), buffer, buffer + (buffer_size - stream.avail_out));
    } while (stream.avail_out == 0);

    // Clean up
    inflateEnd(&stream);
    return decompressed_data;
}

// Writes a block of compressed data to a file
void write_compressed_data(const vector<char>& compressed_data, const string& filename)
{
    ofstream outfile(filename, ios::binary);
    outfile.write(compressed_data.data(), compressed_data.size());
}

// Reads a block of compressed data from a file
vector<char> read_compressed_data(const string& filename)
{
    vector<char> compressed_data;
    ifstream infile(filename, ios::binary | ios::ate);
    if (infile.is_open()) {
        const size_t file_size = infile.tellg();
        infile.seekg(0, ios::beg);
        compressed_data.resize(file_size);
        infile.read(compressed_data.data(), file_size);
        infile.close();
    }
    return compressed_data;
}

int main()
{
    // Read input data
    ifstream infile("input.txt");
    stringstream buffer;
    buffer << infile.rdbuf();
    string input_data = buffer.str();

    // Compress input data
    vector<char> compressed_data = compress_data(input_data.c_str(), input_data.size());

    // Write compressed data to disk
    write_compressed_data(compressed_data, "compressed_input.txt");

    // Read compressed data from disk
    vector<char> read_compressed_data = read_compressed_data("compressed_input.txt");

   
