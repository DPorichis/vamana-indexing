#include "io.h"
#include "vamana-utils.h"
#include <bits/stdc++.h>
#include <iostream>
#include <fstream>
#include <numeric>
#include <vector>
#include <map>

using namespace std;

int update_option(string flag, string value, Options opt);
int check_options(Options opt);


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

// Insert binary data into the 2D vector "data"
void readKNN(const string& filename, const int dimensions, vector<vector<uint32_t>>& data) {
    ifstream file;
    file.open(filename, ios::binary);
    
    if (!file.is_open()) {
        cout << "Error opening file: " << filename << endl;
        return;
    }

    uint32_t vectors_count;     // Num of vectors in file
    file.read((char *)&vectors_count, sizeof(uint32_t));

    data.resize(vectors_count);

    vector<uint32_t> buffer(dimensions);

    int i = 0;
    while (file.read((char*)buffer.data(), dimensions * sizeof(uint32_t))) {
        vector<uint32_t> temp(dimensions);
        for (int j = 0; j < dimensions; j++) {
            temp[j] = static_cast<uint32_t>(buffer[j]);
        }
        data[i++] = move(temp);
    }
    // Close file
    file.close();
}

// Export k nearest neighbours to a file
void saveKNN(vector<vector<uint32_t>>& neighbours, const string& path) {
    ofstream file(path, ios::out | ios::binary);
    int K = 100;
    uint32_t q_count = neighbours.size();
    // K and the size of a KNN's element must be the same
    if (K != neighbours[0].size()) {
        printf("K and KNN not the same size..Error!\n");
        return;
    }
    file.write(reinterpret_cast<const char*>(&q_count), sizeof(uint32_t));

    for (int i = 0; i < q_count; i++) {
        auto const& temp_n = neighbours[i];
        file.write(reinterpret_cast<char const *>(&temp_n[0]), K * sizeof(uint32_t)); 
    }
    file.close();
}

// Save graph to binary file
void saveGraph(Graph graph, const string& output_file) {
    ofstream file(output_file, ios::binary);
    if (!file) {
        cerr << "Error opening file: " << output_file << " for saving graph" << endl;
        return;
    }
    

    // Write type, k, dimensions and unfiltered_medoid of graph 
    file.write(reinterpret_cast<const char*>(&graph->type), sizeof(graph->type));
    file.write(reinterpret_cast<const char*>(&graph->k), sizeof(graph->k));
    file.write(reinterpret_cast<const char*>(&graph->dimensions), sizeof(graph->dimensions));
    file.write(reinterpret_cast<const char*>(&graph->unfiltered_medoid), sizeof(graph->unfiltered_medoid));

    // Write the number of nodes
    size_t node_count = graph->nodes.size();
    file.write(reinterpret_cast<const char*>(&node_count), sizeof(node_count));

    // Write each node
    for (const auto& node : graph->nodes) {
        // Write 'pos' and 'd_count'
        file.write(reinterpret_cast<const char*>(&node->pos), sizeof(node->pos));
        file.write(reinterpret_cast<const char*>(&node->d_count), sizeof(node->d_count));

        // Write components
        file.write(reinterpret_cast<const char*>(node->components), node->d_count * sizeof(float));

        // Write neighbours set
        size_t neighbour_count = node->neighbours.size();
        file.write(reinterpret_cast<const char*>(&neighbour_count), sizeof(neighbour_count));
        for (const Link& link : node->neighbours) {
            file.write(reinterpret_cast<const char*>(&link->to->pos), sizeof(link->to->pos));
        }

        // Write categories set
        size_t category_count = node->categories.size();
        file.write(reinterpret_cast<const char*>(&category_count), sizeof(category_count));
        for (const int category : node->categories) {
            file.write(reinterpret_cast<const char*>(&category), sizeof(category));
        }
    }
    // Write `all_categories`
    int category_count = graph->all_categories.size();
    file.write(reinterpret_cast<const char*>(&category_count), sizeof(category_count));
    for (int category : graph->all_categories) {
        file.write(reinterpret_cast<const char*>(&category), sizeof(category));
    }

    // Write 'medoid_mapping'
    size_t medoids_size = graph->medoid_mapping.size();
    file.write(reinterpret_cast<const char*>(&medoids_size), sizeof(medoids_size));

    // Write each key-value pair in the medoids map
    for (const auto& [key, value] : graph->medoid_mapping) {
        file.write(reinterpret_cast<const char*>(&key), sizeof(key));
        file.write(reinterpret_cast<const char*>(&value), sizeof(value));
    }

    file.close();
}

// Read graph from binary file
void readGraph(Graph& graph, const string& input_file) {
    ifstream file(input_file, ios::binary);
    if (!file) {
        cerr << "Error opening file: " << input_file << " for reading" << endl;
        return;
    }

    // Read type, k, and dimensions
    file.read(reinterpret_cast<char*>(&graph->type), sizeof(graph->type));
    file.read(reinterpret_cast<char*>(&graph->k), sizeof(graph->k));
    file.read(reinterpret_cast<char*>(&graph->dimensions), sizeof(graph->dimensions));
    file.read(reinterpret_cast<char*>(&graph->unfiltered_medoid), sizeof(graph->unfiltered_medoid));

    // Read the number of nodes
    size_t node_count;
    file.read(reinterpret_cast<char*>(&node_count), sizeof(node_count));
    // Clear any existing nodes and reserve space
    graph->nodes.clear();
    graph->nodes.reserve(node_count);

    vector<vector<int>> neighbors_positions(node_count);

    // Read each node
    for (size_t i = 0; i < node_count; ++i) {
        Node node = new struct node;

        file.read(reinterpret_cast<char*>(&node->pos), sizeof(node->pos));
        file.read(reinterpret_cast<char*>(&node->d_count), sizeof(node->d_count));
        
        // Allocate and read components
        node->components = malloc(node->d_count * sizeof(float)); // Adjust type if not float
        if (!node->components) {
            cerr << "Error allocating memory for node components" << endl;
            return;
        }
        file.read(reinterpret_cast<char*>(node->components), node->d_count * sizeof(float));
        
        // Read neighbours set
        size_t neighbor_count;
        file.read(reinterpret_cast<char*>(&neighbor_count), sizeof(neighbor_count));
        
        neighbors_positions[i].resize(neighbor_count);
        file.read(reinterpret_cast<char*>(neighbors_positions[i].data()), neighbor_count * sizeof(int));
        
        // Read categories set
        size_t category_count;
        file.read(reinterpret_cast<char*>(&category_count), sizeof(category_count));
        for (size_t j = 0; j < category_count; ++j) {
            int category;
            file.read(reinterpret_cast<char*>(&category), sizeof(category));
            node->categories.insert(category);
        }

        graph->nodes.push_back(node);
    }
    // Read `all_categories`
    int category_count;
    file.read(reinterpret_cast<char*>(&category_count), sizeof(category_count));
    graph->all_categories.clear();
    for (int i = 0; i < category_count; ++i) {
        int category;
        file.read(reinterpret_cast<char*>(&category), sizeof(category));
        graph->all_categories.insert(category);
    }

    // Read the size of the medoids map
    size_t medoids_size;
    file.read(reinterpret_cast<char*>(&medoids_size), sizeof(medoids_size));

    // Clear and read each key-value pair in the medoids map
    graph->medoid_mapping.clear();
    for (size_t i = 0; i < medoids_size; ++i) {
        int key, value;
        file.read(reinterpret_cast<char*>(&key), sizeof(key));
        file.read(reinterpret_cast<char*>(&value), sizeof(value));
        graph->medoid_mapping[key] = value;
    }

    // Second pass : Updating neighbors
    for (int i = 0; i < node_count; ++i) {
        Node node = graph->nodes[i];
        node->neighbours.clear();
        for (int neighbor_pos : neighbors_positions[i]) {
            Link link = create_link(graph, graph->nodes[i], graph->nodes[neighbor_pos]);
            node->neighbours.insert(link);
        }
    }

    file.close();
}

// Create graph from dataset. Returns graph for success, NULL otherwise
Graph create_graph_from_file(const string& filename, int type, int k, int dimensions) {
    // Store file data to 2D vector
    vector<vector<float>> nodes;
    readBinary(filename, dimensions + 2, nodes);
    //  Graph creation
    Graph graph = create_graph(type, k, dimensions);
    // Insert graph nodes
    for (int i = 0; i < nodes.size(); i++) {
        // Allocate the required memory
        void* components = malloc(dimensions * sizeof(float));
        if (components == NULL) {
        cerr << "Error allocating memory for graph nodes from file" << endl;
        return NULL;
    }
        // Copy vector data to graph (Important)
        memcpy(components, nodes[i].data() + 2, dimensions * sizeof(float));

        // Save the filter in the categories set
        set<int> categories;
        categories.insert(nodes[i][0]);
        add_node_graph(graph, dimensions, components, i, categories);
    }

    // Connecting the nodes
    init_dummy_graph(graph);

    return graph;
}

// Create graph from dataset. Returns graph for success, NULL otherwise
map<int, Graph>* create_stiched_graph_from_file(const string& filename, int type, int k, int dimensions) {
    // Store file data to 2D vector
    vector<vector<float>> nodes;
    readBinary(filename, dimensions + 2, nodes);
    map<int, Graph>* stiched_index = new map<int, Graph>();
    
    // Insert graph nodes
    for (int i = 0; i < nodes.size(); i++) {

        // Allocate the required memory
        void* components = malloc(dimensions * sizeof(float));
        if (components == NULL) {
            cerr << "Error allocating memory for graph nodes from file" << endl;
            return NULL;
        }

        // Copy vector data to graph (Important)
        memcpy(components, nodes[i].data() + 2, dimensions * sizeof(float));

        int category = nodes[i][0];

        if(stiched_index->find(category) == stiched_index->end())
        {
            // Graph creation
            Graph graph = create_graph(type, k, dimensions);
            (*stiched_index)[category] = graph;
        }
        add_node_graph((*stiched_index)[category], dimensions, components, i);
    }

    return stiched_index;
}

// Performs (and allocates) query. Returns the file position of query for success, -1 otherwise
Node ask_query(const std::string& filename, int& type, int dimensions, int& pos) {
    // Store file data to vector
    // vector<file_vector_float> vectors = read_float_vectors_from_file(filename);
    vector<vector<float>> queries;
    readBinary(filename, dimensions + 4, queries);

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
    memcpy(query->components, queries[pos].data() + 4, query->d_count * sizeof(float));
    query->pos = pos;
    query->categories.insert(queries[pos][1]);

    type = queries[pos][0];

    return query;  
}

// Creates file with KNN for recall calculation using sampling 
void create_groundtruth_file(const string& source_file, const string& queries_file, const string& output_file) {
    int data_dimensions = 102;

    // Read data
    vector<vector<float>> nodes;
    readBinary(source_file, data_dimensions, nodes);

    // Read queries
    vector<vector<float>> queries;
    readBinary(queries_file, data_dimensions + 2, queries);

    // To save KNN results
    vector<vector<uint32_t>> knn;

    // Find 100 Nearest Neighbours
    int k = 100;

    for (unsigned int i = 0; i < queries.size(); i++) {
        uint32_t query_type = queries[i][0];
        int v = queries[i][1];
        float l = queries[i][2];
        float r = queries[i][3];
        vector<float> query_values;

        // Push back 2 zeros for aligning with the vectors of dataset
        query_values.push_back(0);
        query_values.push_back(0);
        // Push the other values
        for(int j = 4; j < queries[i].size(); j++) {
            query_values.push_back(queries[i][j]);
        }

        vector<uint32_t> knn_candidate;

        // Different handling according to query type
        if (query_type == 0) {
            for (uint32_t j = 0; j < nodes[i].size(); j++) {
                knn_candidate.push_back(j);
            }
        }
        else if (query_type == 1) {
            for (uint32_t j = 0; j < nodes[i].size(); j++) {
                if (nodes[j][0] == v) {
                    knn_candidate.push_back(j);
                }
            }
        }
        else if (query_type == 2) {
            for (uint32_t j = 0; j < nodes[i].size(); j++) {
                if (nodes[j][1] >= l && nodes[j][1] <= r) {
                    knn_candidate.push_back(j);
                }
            }
        }
        else if (query_type == 3) {
            for (uint32_t j = 0; j < nodes[i].size(); j++) {
                if (nodes[j][0] == v && nodes[j][1] >= l && nodes[j][1] <= r) {
                    knn_candidate.push_back(j);
                }
            }
        }

        // If KNN is less than K, fill with last nodes
        if (knn_candidate.size() < k) {
            uint32_t s = 1;
            while (knn_candidate.size() < k) {
                knn_candidate.push_back(nodes.size() - s);
                s++;
            }
        }

        // Calculate distances between query_values 
        vector<float> distances;
        distances.resize(knn_candidate.size());
        for (uint32_t j = 0; j < knn_candidate.size(); j++) {
            distances[j] = compare_with_id(nodes[knn_candidate[j]], query_values);
        }

        vector<uint32_t> positions;
        positions.resize(knn_candidate.size());
        iota(positions.begin(), positions.end(), 0);
        sort(positions.begin(), positions.end(), [&](uint32_t a, uint32_t b) {
            return distances[a] < distances[b];
        });
        vector<uint32_t> knn_sorted;
        knn_sorted.resize(k);
        for (uint32_t j = 0; j < k; j++) {
            knn_sorted[j] = knn_candidate[positions[j]];
        }
        knn.push_back(knn_sorted);
    }
    // Save the results
    saveKNN(knn, output_file);
    return;

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



float compare_with_id(const std::vector<float>& a, const std::vector<float>& b) {
    float sum = 0.0;
    // Skip the first 2 dimensions
    for (size_t i = 2; i < a.size(); ++i) {
        float diff = a[i] - b[i];
        sum += diff * diff;
    }
    return sum;
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
    cout << "- Index type: " << opt->index_type << endl;
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
    else if(flag == "savegraph")
    {
        if(value[0] == 't')
            opt->savegraph = true;
        else
            opt->savegraph = false;
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
    else if(flag == "index") {
        if(value[0] != 'f' && value[0] != 's' && value[0] != 'u') {
            cout << "Invalid index: must be f/s/u" << endl;
        }
        opt->index_type = value[0];
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