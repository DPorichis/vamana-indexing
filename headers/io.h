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


struct options {

    // Type of file
    // 0 for simple data file
    // 1 for graph file
    int file_type;
    
    std::string data_filename;
    char data_type;

    std::string queries_filename;
    int query_count;
    
    std::string truth_filename;

    float a;
    int k;
    int L;
    int R;

    bool printing;
    bool savegraph;
    
    // Basic Constractor
    options()
        : file_type(0), data_filename(""), data_type('f'), queries_filename(""), query_count(1), truth_filename(""),
        a(1), k(1), L(1), R(1), printing(true), savegraph(false) {}
};

typedef struct options* Options;

// Inserting data in the library structure
vector<file_vector_float> read_float_vectors_from_file(const string& filename);

vector<file_vector_int> read_int_vectors_from_file(const string& filename);

vector<file_vector_char> read_char_vectors_from_file(const string& filename);

// Create graph from dataset. Returns graph for success, NULL otherwise
Graph create_graph_from_file(const string& filename, int type, int k);

// Performs (and allocates) query. Returns the query as a node for success, NULL otherwise
Node ask_query(const string& filename, int type, int graph_dimension, int& pos);

// Function for reading command line arguments
int read_command_line_args(int argc, char* argv[], Options opt);

// Function for reading the configuration file
int read_config_file(string filename, Options opt);

// Prints out the options for debugging and reporting purposes
void print_options(Options opt);

// Release all memory for exiting.....I dont think we need this...
int destroy_data(void);