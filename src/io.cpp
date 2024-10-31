#include "io.h"
#include <bits/stdc++.h>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;


vector<file_vector> read_vectors_from_file(const std::string& filename) {
    vector<file_vector> vectors;
    ifstream infile(filename, ios::binary);

    if (!infile) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return vectors; // Return empty vector on error
    }

    while (infile.peek() != EOF) { // Check if end of file
        file_vector vec;
        
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

Graph create_graph_from_file(const string& filename, int type, int k) {
    // Store file data to vector
    vector<file_vector> vectors = read_vectors_from_file(filename);
    //  Graph creation
    Graph graph = create_graph(type, k, vectors[0].d);

    // Insert graph nodes
    for (int i = 0; i < vectors.size(); i++) {
        // Allocate the required memory. We use max in order to secure that we have enough space to store the data
        // For example, if a dataset has floats and chars, we want an array of floats that have enough space to store chars as well
        void* components = malloc(graph->dimensions * sizeof(*max_element(vectors[i].components.begin(), vectors[i].components.end())));
        memcpy(components, vectors[i].components.data(), graph->dimensions * sizeof(*max_element(vectors[i].components.begin(), vectors[i].components.end())));    // Copy vector data to graph (Important)
        add_node_graph(graph, vectors[i].d, components);
    }

    // Connecting the nodes
    init_dummy_graph(graph);

    return graph;
}

