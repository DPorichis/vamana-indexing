#pragma once
#include <vector>
#include <string>
#include "vamana.h"
#include "graph.h"

using namespace std;

// Useful struct to store data from the datasets
struct file_vector {
    int d;                          // Dimension of the vector
    std::vector<float> components;  // Vector components
};

struct file_vector2 {
    int d;                          // Dimension of the vector
    std::vector<int> components;  // Vector components
};


// Inserting data in the library structure
vector<file_vector> read_vectors_from_file(const string& filename);

vector<file_vector2> read_int_vectors_from_file(const string& filename);

// Create graph from dataset. Returns graph for success, NULL otherwise
Graph create_graph_from_file(const string& filename, int type, int k);

// Performs (and allocates) query. Returns the query as a node for success, NULL otherwise
Node ask_query(const string& filename, int graph_dimension, int& pos);

// Release all memory for exiting.....I dont think we need this...
int destroy_data(void);