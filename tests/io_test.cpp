#include "acutest.h"
#include <iostream>
#include "graph.h"
#include "io.h"
#include <iomanip>

using namespace std;

void test_create_from_file(void) {
	// Test with the file "siftsmall_base.fvecs"
	string path = "../data/siftsmall/siftsmall_base.fvecs";

	// Create graph
	Graph graph = create_graph_from_file(path, 'f', 5);

	// The vectors of the dataset
	vector<file_vector> vectors = read_vectors_from_file(path);
	
	TEST_ASSERT(graph != NULL);
	TEST_ASSERT(graph->type == 'f');
	TEST_ASSERT(graph->k == 5);

	// Checks that all vectors from the file have the same dimesnion
	int dimension = vectors[0].d;
	for (int i = 0; i < vectors.size(); i++) {
		TEST_ASSERT(dimension == vectors[i].d);
	}
	TEST_ASSERT(dimension == graph->dimensions);

	// Checks that nodes from file have been inserted correctly
	TEST_ASSERT(graph->nodes.size() == vectors.size());

	// Checks that vector node no.0 is in the same position on the graph
	for (int i = 0; i < graph->dimensions; i++) {
		TEST_ASSERT(((float*)graph->nodes[0]->components)[i] == vectors[0].components[i]);
	}

	// Also check for a node at random position between 0 - graph_size
	srand(static_cast<unsigned int>(time(0)));
	int pos = rand() % graph->nodes.size();
	for (int i = 0; i < graph->dimensions; i++) {
		TEST_ASSERT(((float*)graph->nodes[pos]->components)[i] == vectors[pos].components[i]);
	}

	// Destroy graph
	destroy_graph(graph);

}

void test_query(void) {
	// Test with the file "siftsmall_query.fvecs"
	string path = "../data/siftsmall/siftsmall_query.fvecs";

	// Create graph
	Graph graph = create_graph_from_file("../data/siftsmall/siftsmall_base.fvecs", 'f', 5);	

	// The vectors of the query dataset
	vector<file_vector> vectors = read_vectors_from_file(path);

	// The file position of query. Gets value by user input
	int pos; 

	// Call function
	Node query = ask_query(path, graph->dimensions, pos);
	
	TEST_ASSERT(query != NULL);

	// Check if there is a match between the vector node in pos position and the returned node by the function
	TEST_ASSERT(vectors[pos].d == query->d_count);

	for (int i = 0; i < query->d_count; i++) {
		TEST_ASSERT(vectors[pos].components[i] == ((float*)query->components)[i]);
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

TEST_LIST = {
	{ "list_create", test_create_from_file },
	{ "perform_query", test_query},
	{ NULL, NULL }
};