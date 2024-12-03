#include "io.h"
#include <bits/stdc++.h>
#include <iostream>
#include <fstream>
#include <vector>
using namespace std;


// Insert binary data into the 2D vector "data"
void readBinary(const string& filename, const int dimensions, vector<vector<float>>& data) {
    ifstream file;
    file.open(filename, ios::binary);
    
    if (!file.is_open()) {
        cout << "Error opening file: " << filename << endl;
        return;
    }

    uint32_t vectors_count;     // Num of vectors in file
    file.read((char *)&vectors_count, sizeof(uint32_t));

    data.resize(vectors_count);

    vector<float> buffer(dimensions);

    int i = 0;
    while (file.read((char*)buffer.data(), dimensions * sizeof(float))) {
        vector<float> temp(dimensions);
        for (int j = 0; j < dimensions; j++) {
            temp[j] = static_cast<float>(buffer[j]);
        }
        data[i++] = move(temp);
    }
    // Close file
    file.close();
}


vector<file_vector_float> read_float_vectors_from_file(const std::string& filename) {
    vector<file_vector_float> vectors;
    ifstream infile(filename, ios::binary);

    if (!infile) {
        cerr << "Error opening file: " << filename << endl;
        return vectors; // Return empty vector on error
    }

    while (infile.peek() != EOF) { // Check if end of file
        file_vector_float vec;
        
        // Read the dimension
        infile.read(reinterpret_cast<char*>(&vec.d), sizeof(vec.d));
        if (infile.eof()) break; // Break if we reach EOF
        
        // Allocate the correct size for components
        vec.components.resize(vec.d);
        
        // Read the components
        infile.read(reinterpret_cast<char*>(vec.components.data()), vec.d * sizeof(float));
        
        vectors.push_back(vec); // Store the vector
    }

    infile.close(); // Close the file
    return vectors; // Return the vector list
}


// Create graph from dataset. Returns graph for success, NULL otherwise
Graph create_graph_from_file(const string& filename, int type, int k, int dimensions) {
    // Store file data to 2D vector
    vector<vector<float>> nodes;
    readBinary(filename, dimensions, nodes);
    //  Graph creation
    Graph graph = create_graph(type, k, dimensions);

    // Insert graph nodes
    for (int i = 0; i < nodes.size(); i++) {
        // Allocate the required memory. We use max in order to secure that we have enough space to store the data
        // For example, if a dataset has floats and chars, we want an array of floats that have enough space to store chars as well
        void* components = malloc(dimensions * sizeof(float));
        if (components == NULL) {
        cerr << "Error allocating memory for graph nodes from file" << endl;
        return NULL;
    }
        // Copy vector data to graph (Important)
        memcpy(components, nodes[i].data(), dimensions * sizeof(float));
        add_node_graph(graph, dimensions, components, i);
    }

    // Connecting the nodes
    init_dummy_graph(graph);

    return graph;
}

// Performs (and allocates) query. Returns the file position of query for success, -1 otherwise
Node ask_query(const std::string& filename, int type, int dimensions, int& pos) {
    // Store file data to vector
    // vector<file_vector_float> vectors = read_float_vectors_from_file(filename);
    vector<vector<float>> queries;
    readBinary(filename, dimensions, queries);

    // Random query
    pos = rand() % queries.size();

    if (pos < 0 || pos > queries.size() - 1) {
        cerr << "Position outside of range" << endl;
        return NULL;
    }
    
    // Allocate memory for node
    Node query = new node;

    // Node initialization
    query->d_count = dimensions;
    query->components = malloc(query->d_count * sizeof(float));
    memcpy(query->components, queries[pos].data(),query->d_count * sizeof(float));
    query->pos = pos;

    return query;  
}

vector<file_vector_int> read_int_vectors_from_file(const string& filename) {
    vector<file_vector_int> vectors;
    ifstream infile(filename, ios::binary);

    if (!infile) {
        cerr << "Error opening file: " << filename << endl;
        return vectors; // Return empty vector on error
    }

    while (infile.peek() != EOF) { // Check if end of file
        file_vector_int vec;
        
        // Read the dimension
        infile.read(reinterpret_cast<char*>(&vec.d), sizeof(vec.d));
        if (infile.eof()) break; // Break if we reach EOF
        
        // Allocate the correct size for components
        vec.components.resize(vec.d);
        
        // Read the components
        infile.read(reinterpret_cast<char*>(vec.components.data()), vec.d * sizeof(int));
        
        vectors.push_back(vec); // Store the vector
    }

    infile.close(); // Close the file
    return vectors; // Return the vector list
}

vector<file_vector_char> read_char_vectors_from_file(const string& filename) {
    vector<file_vector_char> vectors;
    ifstream infile(filename, ios::binary);

    if (!infile) {
        cerr << "Error opening file: " << filename << endl;
        return vectors; // Return empty vector on error
    }

    while (infile.peek() != EOF) { // Check if end of file
        file_vector_char vec;
        
        // Read the dimension
        infile.read(reinterpret_cast<char*>(&vec.d), sizeof(vec.d));
        if (infile.eof()) break; // Break if we reach EOF
        
        // Allocate the correct size for components
        vec.components.resize(vec.d);
        
        // Read the components
        infile.read(reinterpret_cast<char*>(vec.components.data()), vec.d * sizeof(char));
        
        vectors.push_back(vec); // Store the vector
    }

    infile.close(); // Close the file
    return vectors; // Return the vector list
}