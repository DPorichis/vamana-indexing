#pragma once
#include <vector>
#include <string>
#include "vamana.h"
#include "graph.h"

struct file_vector {
    int d;                          // Dimension of the vector
    std::vector<float> components;  // Vector components
};

// Inserting data in the library structure
vector<file_vector> read_vectors_from_file(const std::string& filename);

// Create graph from dataset
Graph create_graph_from_file(const std::string& filename, int type, int k);

// Performs query and prints out the results
int ask_query(void * data);

// Release all memory for exiting
int destroy_data(void);