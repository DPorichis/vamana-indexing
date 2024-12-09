#pragma once
#include <vector>
#include <string>
#include "vamana.h"
#include "graph.h"
#include "filtered-vamana.h"
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

    char index_type;
    
    // Basic Constractor
    options()
        : file_type(0), data_filename(""), data_type('f'), queries_filename(""), query_count(1), truth_filename(""),
        a(1), k(1), L(1), R(1), printing(true), savegraph(false), index_type('f') {}
};

typedef struct options* Options;

/*Insert data from binary file to 2D vector "data"
    2D vector contains:
        vectors with the following formation:
            ((2 or 4 values) + node_dimensions)
              |    |                
            data  query                         */
void readBinary(const string& filename, const int dimensions, vector<vector<float>>& data);

void readSmallBinary(const string& filename, const int dimensions, vector<vector<float>>& data, int nodes_count);

void writeBinary(const string& filename, const int dimensions, vector<vector<float>>& data);

// Grountruth data into 'data'
void readKNN(const string& filename, const int dimensions, vector<vector<uint32_t>>& data);

/*  Export k nearest neighbours to a file in the following format:
       num_of_queries * vector with size k with the position of the nearest neighbours */
void saveKNN(vector<vector<uint32_t>>& neighbours, const string& path);

// Save graph to binary file
void saveGraph(Graph graph, const string& output_file);

// Read graph from binary file
void readGraph(Graph& graph, const string& input_file);

// Inserting data in the library structure
vector<file_vector_float> read_float_vectors_from_file(const string& filename);

vector<file_vector_int> read_int_vectors_from_file(const string& filename);

vector<file_vector_char> read_char_vectors_from_file(const string& filename);

// Create graph from dataset. Returns graph for success, NULL otherwise
Graph create_graph_from_file(const string& filename, int type, int k, int dimensions);

map<int, Graph>* create_stiched_graph_from_file(const string& filename, int type, int k, int dimensions);

// Performs (and allocates) query. Returns the query as a node for success, NULL otherwise
// type: 0 -> vector-only query, 1 -> query with categorical value , 2 -> query with timestamp, 3 -> categorical and timestamp
Node ask_query(const string& filename, int& type, int graph_dimension, int& pos);

// Creates file with KNN for recall calculation using sampling 
void create_groundtruth_file(const string& source_file, const string& queries_file, const string& output_file);

float compare_with_id(const std::vector<float>& a, const std::vector<float>& b);

// Function for reading command line arguments
int read_command_line_args(int argc, char* argv[], Options opt);

// Function for reading the configuration file
int read_config_file(string filename, Options opt);

// Prints out the options for debugging and reporting purposes
void print_options(Options opt);


// Release all memory for exiting.....I dont think we need this...
int destroy_data(void);