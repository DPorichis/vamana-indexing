#include "io.h"
#include <bits/stdc++.h>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

int update_option(string flag, string value, Options opt);
int check_options(Options opt);


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

int read_config_file(string filename, Options opt)
{
    std::ifstream inputFile(filename);

    // Error in opening
    if (!inputFile) {
        std::cerr << "Error opening file " << filename << std::endl;
        delete opt;
        return -1;
    }

    std::string line;
    
    // Read the file line by line
    while (std::getline(inputFile, line)) {
        
        // Skip comments and empty lines
        if (line.empty() || line[0] == '#') {
            cout << "skipping";
            continue;
        }

        // Split the line in two (flag)=(value)
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string flag = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            if(update_option(flag, value, opt) == -1)
            {
                inputFile.close();
                return -1;            
            }   
        }
    }
    inputFile.close();

    if(check_options(opt) == -1)
    {
        return -1;
    }

    return 0;
}

int read_command_line_args(int argc, char* argv[], Options opt)
{
    // Read the file line by line
    for(int i = 1; i < argc; i++) {

        string line = argv[i];
        // Split the line in two (flag)=(value)
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string flag = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            if(update_option(flag, value, opt) == -1)
            {
                return -1;            
            }   
        }
    }
    if(check_options(opt) == -1)
    {
        return -1;
    }

    return 0;
}


void print_options(Options opt)
{
    cout << "--- Options overview ---" << endl;
    cout << "- Data file: " << opt->data_filename << endl;
    cout << "- Data type: " << opt->data_type << endl;
    if(opt->file_type == 0)
        cout << "- File type: Raw-data File" << endl;
    else
        cout << "- File type: Graph File" << endl;
    cout << "----" << endl;
    cout << "- Queries file: " << opt->queries_filename << endl;
    cout << "- Queries to be performed: " << opt->query_count << endl;
    cout << "----" << endl;
    if(opt->truth_filename.compare("") == 0)
        cout << "- No ground truth file provided, accuracy stats won't be calculated" << endl;
    else
        cout << "- Groundtruth file: " << opt->truth_filename << endl;
    cout << "----" << endl;
    cout << "- a: " << opt->a << endl;
    cout << "- k: " << opt->k << endl;
    cout << "- L: " << opt->L << endl;
    cout << "- R: " << opt->R << endl;
    cout << "----" << endl;
}

int update_option(string flag, string value, Options opt)
{
    if(flag == "data")
    {
        opt->data_filename = value;
    }
    else if(flag == "filetype")
    {
        if(value[0] == 'd')
            opt->file_type = 0;
        else
            opt->file_type = 1;
    }
    else if(flag == "printing")
    {
        if(value[0] == 't')
            opt->printing = true;
        else
            opt->printing = false;
    }
    else if(flag == "datatype")
    {
        opt->data_type = value[0];
        if(value[0] != 'c' && value[0] != 'f' && value[0] != 'i')
        {
            cout << "Invalid data_type: data_type must be ('c', 'f' or 'i') " << endl;
            return -1;
        }
    }
    else if(flag == "truth")
    {
        opt->truth_filename = value;
    }
    else if(flag == "queries")
    {
        opt->queries_filename = value;
    }
    else if(flag == "queriescount")
    {
        opt->query_count = std::stoi(value);
        if(opt->query_count < 1)
        {
            cout << "Invalid querieCount: querieCount must be >= 1" << endl;
            return -1;
        }
    }
    else if(flag == "R")
    {
        opt->R = std::stoi(value);
        if(opt->R < 1)
        {
            cout << "Invalid R: R must be >= 1" << endl;
            return -1;
        }
    }
    else if(flag == "L")
    {
        opt->L = std::stoi(value);
        if(opt->L < 1)
        {
            cout << "Invalid L: L must be >= 1" << endl;
            return -1;
        }
    }
    else if(flag == "k")
    {
        opt->k = std::stoi(value);
        if(opt->k < 1)
        {
            cout << "Invalid k: k must be >= 1" << endl;
            return -1;
        }
    }
    else if(flag == "a")
    {
        opt->a = std::stof(value);
        if(opt->a < 1)
        {
            cout << "Invalid a: a must be >= 1" << endl;
            return -1;
        }
    }
    return 0;
}



int check_options(Options opt)
{
    int ret = 0;
    
    if(opt->data_filename.compare("") == 0)
    {
        cout << "Error: No datafile provided. Use data=[yourfile]" << endl;
        ret = -1;
    }
    if(opt->queries_filename.compare("") == 0)
    {
        cout << "Error: No queries file provided. Use queries=[yourfile] " << endl;
        ret = -1;
    }
    return ret;
}