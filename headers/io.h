#pragma once
#include <vector>
#include <string>
#include "vamana.h"
#include "graph.h"
#include <cstdint>

using namespace std;

// Useful struct to store data from the datasets
struct file_vector_int {
    int d;                          // Dimension of the vector
    std::vector<int> components;  // Vector components
};

struct file_vector_float {
    int d;                          // Dimension of the vector
    std::vector<float> components;  // Vector components
};

struct file_vector_char {
    int d;                          // Dimension of the vector
    std::vector<char> components;  // Vector components
};  

/*Insert data from binary file to 2D vector "data"
    2D vector contains:
        vectors with the following formation:
            ((2 or 4 values) + node_dimensions)
              |    |                
            data  query                         */
void readBinary(const string& filename, const int dimensions, vector<vector<float>>& data);


/*  Export k nearest neighbours to a file in the following format:
       num_of_queries * vector with size k with the position of the nearest neighbours */
void saveKNN(vector<vector<uint32_t>>& neighbours, const string& path);

// Inserting data in the library structure
vector<file_vector_float> read_float_vectors_from_file(const string& filename);

vector<file_vector_int> read_int_vectors_from_file(const string& filename);

vector<file_vector_char> read_char_vectors_from_file(const string& filename);

// Create graph from dataset. Returns graph for success, NULL otherwise
Graph create_graph_from_file(const string& filename, int type, int k, int dimensions);

// Performs (and allocates) query. Returns the query as a node for success, NULL otherwise
Node ask_query(const string& filename, int type, int graph_dimension, int& pos);

// Creates file with KNN for recall calculation using sampling 
void create_groundtruth_file(const string& source_file, const string& queries_file, const string& output_file);

float compare_with_id(const std::vector<float>& a, const std::vector<float>& b);

// Release all memory for exiting.....I dont think we need this...
int destroy_data(void);