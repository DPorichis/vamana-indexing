#include "acutest.h"
#include <iostream>
#include "graph.h"
#include "io.h"
#include <iomanip>

using namespace std;

void test_create_from_file(void) {
	// Test with the file "../data/dummy-data.bin"
	string path = "../data/dummy-data.bin";
	int dimensions = 100;

	// Create graph
	Graph graph = create_graph_from_file(path, 'f', 5, dimensions);

	// The vectors of the dataset
	vector<vector<float>> data;
	readBinary(path, dimensions + 2, data);
	
	TEST_ASSERT(graph != NULL);
	TEST_ASSERT(graph->type == 'f');
	TEST_ASSERT(graph->k == 5);

	// Checks that all vectors from the file have the same dimension
	for (int i = 0; i < data.size(); i++) {
		TEST_ASSERT(dimensions == data[i].size() - 2);
	}
	TEST_ASSERT(dimensions == graph->dimensions);

	// Checks that nodes from file have been inserted correctly
	TEST_ASSERT(graph->nodes.size() == data.size());

	// Checks that vector node no.0 is in the same position on the graph
	for (int i = 0; i < graph->dimensions; i++) {
		TEST_ASSERT(((float*)graph->nodes[0]->components)[i] == data[0][i + 2]);
	}

	// Also check for a node at random position between 0 - graph_size
	srand(static_cast<unsigned int>(time(0)));
	int pos = rand() % graph->nodes.size();
	for (int i = 0; i < graph->dimensions; i++) {
		TEST_ASSERT(((float*)graph->nodes[pos]->components)[i] == data[pos][i + 2]);
	}

	// Destroy graph
	destroy_graph(graph);

}

void test_query(void) {
	// Test with the file "../data/dummy-queries.bin"
	string path = "../data/dummy-queries.bin";
	int dimensions = 100;

	// Create graph
	Graph graph = create_graph_from_file("../data/dummy-data.bin", 'f', 5, dimensions);

	// The vectors of the query dataset
	vector<vector<float>> data;
	readBinary(path, dimensions + 4, data);			// Queries: 104 dimensions

	// The file position of query. Gets value by user input
	int pos; 

	// Call function
	Node query = ask_query(path, graph->type, graph->dimensions, pos);
	
	TEST_ASSERT(query != NULL);

	for (int i = 0; i < query->d_count; i++) {
		TEST_ASSERT(data[pos][i + 4] == ((float*)query->components)[i]);
	}

	// ********* UNCOMMENT if we want user input ***************
	// Edits the terminal output for better viewing
	// cout << "\033[F\033[F"; 
    // cout << "\033[K";
    // cout << "\033[B\033[K"; 
	// cout << setw(52);
	// cout << "\033[2A";

	// Destroy the query node and the graph
	destroy_node(query);
	destroy_graph(graph);
}

void test_groundtruth(void) {
	
	string source_file = "../data/dummy-data.bin";
	string queries_file = "../data/dummy-queries.bin";
	string output_file = "../data/groundtruth.bin";
	// int dimensions = 102;
	// // Create graph
	// Graph graph = create_graph_from_file(path, 'f', 5, dimensions);

	// The vectors of the dataset
	// vector<vector<float>> data;
	// readBinary(source_file, 102, data);

	// create_groundtruth_file(source_file, queries_file, output_file);
	// TEST_ASSERT()
	
}

void test_save_write(void) {
	string path = "../data/dummy-data.bin";
	int dimensions = 100;

	// Create graph
	Graph graph = create_graph_from_file(path, 'f', 5, dimensions);

	saveGraph(graph, "../data/graph.bin");

	Graph new_graph = create_graph(0, 0, 0);
	readGraph(new_graph, "../data/graph.bin");
	TEST_ASSERT(new_graph->type == 'f');
	TEST_ASSERT(new_graph->dimensions == dimensions);
	TEST_ASSERT(new_graph->k == 5);
	TEST_ASSERT(graph->unfiltered_medoid == new_graph->unfiltered_medoid);
	TEST_ASSERT(graph->nodes.size() == new_graph->nodes.size());
	
	// Check that components of a random position are the same
	int pos = rand() % graph->nodes.size();

	for (int i = 0; i < graph->dimensions; i++) {
		TEST_ASSERT(((float*)graph->nodes[pos]->components)[i] == ((float*)new_graph->nodes[pos]->components)[i]);
	}
	
	// Checks that nodes of random position have the same neigbhors
	pos = rand() % graph->nodes.size();

	TEST_ASSERT(graph->nodes[pos]->neighbours.size() == new_graph->nodes[pos]->neighbours.size());

	// Extract neighbors of the first graph's node
	set<int> neighbors1;
	for (const Link& link : graph->nodes[pos]->neighbours) {
		neighbors1.insert(link->to->pos);
	}
	// Extract the neighbors of the second graph's node
	set<int> neighbors2;
	for (const Link& link : new_graph->nodes[pos]->neighbours) {
		neighbors2.insert(link->to->pos);
	}
	// Check that they have the same neighbors
	TEST_ASSERT(neighbors1 == neighbors2);

	// Check that medoid map has the same size
	TEST_ASSERT(graph->medoid_mapping.size() == new_graph->medoid_mapping.size());

	// Check that all categories have the same size
	TEST_ASSERT(graph->all_categories.size() == new_graph->all_categories.size());

	destroy_graph(graph);
	destroy_graph(new_graph);
}


TEST_LIST = {
	{ "create_graph_from_file", test_create_from_file },
	{ "perform_query", test_query},
	{ "create_groundtruth_file", test_groundtruth},
	{ "save/write_graph_to_file", test_save_write},
	{ NULL, NULL }
};