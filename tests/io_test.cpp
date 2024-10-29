#include "acutest.h"
#include <iostream>
#include "graph.h"
#include "io.h"

using namespace std;

void test_create_from_file(void) {
	// Test with the file "siftsmall_base.fvecs"
	string path = "../data/siftsmall/siftsmall_base.fvecs";
	Graph graph = create_graph_from_file(path, 'f', 5);

	// The vectors of the dataset
	vector<file_vector> vectors = read_vectors_from_file(path);
	
	TEST_ASSERT(graph != NULL);
	TEST_ASSERT(graph->type == 'f');
	TEST_ASSERT(graph->k == 5);

	// Checks that all vectors from the filehave the same dimesnion
	int dimension = vectors[0].d;
	for (int i = 0; i < vectors.size(); i++) {
		TEST_ASSERT(dimension == vectors[i].d);
	}
	TEST_ASSERT(dimension == graph->dimensions);

	// Checks that nodes from file have been inserted correctly
	TEST_ASSERT(graph->nodes.size() == vectors.size());

	// Check if there is a match between the data of the dataset and the graph
	// Node no.0
	TEST_ASSERT(graph->nodes[0]->d_count == vectors[0].d);
	for (int i = 0; i < graph->nodes[0]->d_count; i++) {
		cout << ((float*)graph->nodes[0]->components)[i] << " " << vectors[0].components[i] << endl;

		// vector<float> float_array = *(float*)&graph->nodes[0]->components;
		// TEST_ASSERT(((float*)graph->nodes[0]->components)[i] == vectors[0].components[i]);
		// cout << ((float*)graph->nodes[0]->components)[i] << " " << vectors[0].components[i] << endl;
		// cout << float_array[i] << " " << vectors[0].components[i] << endl;
		// cout << graph->nodes[0]->components << " " << (void*)vectors[0].components.data() << endl;		
	}
	// cout << graph->nodes[0]->components[0] << " " << vectors[0].components << endl;
	// float* float_array = (float*)&graph->nodes[0]->components;
	// cout << float_array[0] << endl;
	// destroy_graph(graph);
}

TEST_LIST = {
	{ "list_create", test_create_from_file },
	{ NULL, NULL }
};