#include "acutest.h"
#include <iostream>
#include "graph.h"
#include "io.h"

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

TEST_LIST = {
	{ "list_create", test_create_from_file },
	{ NULL, NULL }
};