#pragma once
#include <vector>
#include <string>
#include "vamana.h"
#include "graph.h"

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


// Inserting data in the library structure
vector<file_vector_float> read_float_vectors_from_file(const string& filename);

vector<file_vector_int> read_int_vectors_from_file(const string& filename);

vector<file_vector_char> read_char_vectors_from_file(const string& filename);

// Create graph from dataset. Returns graph for success, NULL otherwise
Graph create_graph_from_file(const string& filename, int type, int k);

// Performs (and allocates) query. Returns the query as a node for success, NULL otherwise
Node ask_query(const string& filename, int type, int graph_dimension, int& pos);

// Release all memory for exiting.....I dont think we need this...
int destroy_data(void);