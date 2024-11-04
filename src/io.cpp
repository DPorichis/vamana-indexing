#include "io.h"
#include <bits/stdc++.h>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;


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
Graph create_graph_from_file(const string& filename, int type, int k) {
    // Store file data to vector
    // if (type == 'f')
    //     vector<file_vector_float> vectors = read_float_vectors_from_file(filename);
    // else if (type == 'i')
    //     vector<file_vector_int> vectors = read_int_vectors_from_file(filename);
    // else if (type == 'c')
    //     vector<file_vector_char> vectors = read_char_vectors_from_file(filename);

    vector<file_vector_float> vectors = read_float_vectors_from_file(filename);
    //  Graph creation
    Graph graph = create_graph(type, k, vectors[0].d);

    // Insert graph nodes
    for (int i = 0; i < vectors.size(); i++) {
        // Allocate the required memory. We use max in order to secure that we have enough space to store the data
        // For example, if a dataset has floats and chars, we want an array of floats that have enough space to store chars as well
        void* components = malloc(graph->dimensions * sizeof(*max_element(vectors[i].components.begin(), vectors[i].components.end())));
        if (components == NULL) {
        cerr << "Error allocating memory for graph nodes from file" << endl;
        return NULL;
    }
        // Copy vector data to graph (Important)
        memcpy(components, vectors[i].components.data(), graph->dimensions * sizeof(*max_element(vectors[i].components.begin(), vectors[i].components.end())));    // Copy vector data to graph (Important)
        add_node_graph(graph, vectors[i].d, components, i);
    }

    // Connecting the nodes
    init_dummy_graph(graph);

    return graph;
}


// Performs (and allocates) query. Returns the file position of query for success, -1 otherwise
Node ask_query(const std::string& filename, int type, int graph_dimension, int& pos) {
    // Store file data to vector
    
    if (type == 'f')
        vector<file_vector_float> vectors = read_float_vectors_from_file(filename);
    else if (type == 'i')
        vector<file_vector_int> vectors = read_int_vectors_from_file(filename);
    else if (type == 'c')
        vector<file_vector_char> vectors = read_char_vectors_from_file(filename);
        
    vector<file_vector_float> vectors = read_float_vectors_from_file(filename);


    // Random query
    
    pos = rand() % vectors.size();

    if (pos < 0 || pos > vectors.size() - 1) {
        cerr << "Position outside of range" << endl;
        return NULL;
    }

    //Checks for match between graph and node dimension
    if (vectors[pos].d != graph_dimension) {
        cerr << "Dimension of query doesn't match to graph's" << endl;
        return NULL;
    }
    
    // Allocate memory for node
    Node query = new node;

    // Node initialization
    query->d_count = vectors[pos].d;
    query->components = malloc(query->d_count * sizeof(*max_element(vectors[pos].components.begin(), vectors[pos].components.end())));
    memcpy(query->components, vectors[pos].components.data(), vectors[pos].d * sizeof(*max_element(vectors[pos].components.begin(), vectors[pos].components.end())));
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