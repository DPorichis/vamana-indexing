#include "io.h"
#include <bits/stdc++.h>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;


vector<file_vector> read_vectors_from_file(const std::string& filename) {
    vector<file_vector> vectors;
    ifstream infile(filename);

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

    vector<file_vector> vectors = read_vectors_from_file(filename);
    Graph g = create_graph(type, k, vectors[0].d);

    // Insert graph nodes
    for (int i = 0; i < vectors.size(); i++) {
        void* components = static_cast<void*>(vectors[i].components.data());
        add_node_graph(g, vectors[i].d, components);
        // cout << ((float*)g->nodes[0]->components)[i] << " " << vectors[0].components[i] << endl;

    }

    // Connecting the nodes
    init_dummy_graph(g);
    
    return g;
}

